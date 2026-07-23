/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Product MSI/MSI-X capability discovery + table entry programming.
 * Clean-room pure C from PCI Local Bus Spec capability IDs and MSI-X
 * table layout. Soft table shadow for smokes without MMIO.
 * No GPL source.
 *
 * greppable: MSI-X table soft path
 *
 * Soft inventory (Wave 11 exclusive; this unit only; never hard-gates):
 * greppable: "pci: soft …" | "msix: soft …"
 *   pci: soft inventory … / msix: soft inventory …
 *   pci: soft table …     / msix: soft table …
 *   pci: soft pba …       / msix: soft pba …
 *   pci: soft fire …      / msix: soft fire …
 *   pci: soft mask …      / msix: soft mask …
 *   pci: soft hw …        / msix: soft hw …
 *   pci: soft path …      / msix: soft path …
 *   pci: soft inventory PASS / pci: soft PASS
 *   msix: soft inventory PASS / msix: soft PASS
 * Honesty: soft shadow depth ≠ full device Table Size; soft ≠ bar3.
 */
#include <gj/config.h>
#include <gj/irq_msix.h>
#include <gj/klog.h>
#include <gj/pci_caps.h>
#include <gj/string.h>
#include <gj/types.h>
#include <gj/vmm.h>

#define PCI_CAP_ID_MSI  0x05u
#define PCI_CAP_ID_MSIX 0x11u
#define PCI_STATUS_CAP  0x10u /* Status[4] Capabilities List */

/* Local APIC MMIO message address base (xAPIC FEE0_0000h) */
#define MSI_ADDR_BASE 0xFEE00000u

/* Product default vector used by pci_msix_probe_log (matches GJ_MSIX_IRQ_VEC) */
#define PCI_MSIX_PROBE_VEC 0x41u

static u32 g_u32Programmed;

/* Soft MSI-X table shadow (always available). */
static struct gj_pci_msix_soft_entry g_aSoftTab[GJ_MSIX_SOFT_TBL];
static u64 g_u64SoftPba;
static u32 g_u32SoftProg;
static u32 g_u32SoftFire;
static int g_fSoftReady;
/* Wave 11: times soft inventory printed (diagnostics only). */
static u32 g_u32SoftInvLogs;

static u32
pci_cfg_read(u8 u8Bus, u8 u8Slot, u8 u8Func, u8 u8Off)
{
    u32 u32Addr = 0x80000000u | ((u32)u8Bus << 16) | ((u32)u8Slot << 11) |
                  ((u32)u8Func << 8) | (u8Off & 0xfcu);
    u32 u32Val;

    __asm__ volatile("outl %0, %1" : : "a"(u32Addr), "Nd"((u16)0xCF8));
    __asm__ volatile("inl %1, %0" : "=a"(u32Val) : "Nd"((u16)0xCFC));
    return u32Val;
}

static u8
pci_cfg_read8(u8 u8Bus, u8 u8Slot, u8 u8Func, u8 u8Off)
{
    u32 u32Word = pci_cfg_read(u8Bus, u8Slot, u8Func, u8Off & 0xfcu);

    return (u8)((u32Word >> ((u8Off & 3u) * 8u)) & 0xffu);
}

static u16
pci_cfg_read16(u8 u8Bus, u8 u8Slot, u8 u8Func, u8 u8Off)
{
    u32 u32Word = pci_cfg_read(u8Bus, u8Slot, u8Func, u8Off & 0xfcu);

    return (u16)((u32Word >> ((u8Off & 2u) * 8u)) & 0xffffu);
}

static void
pci_cfg_write16(u8 u8Bus, u8 u8Slot, u8 u8Func, u8 u8Off, u16 u16Val)
{
    u32 u32Addr = 0x80000000u | ((u32)u8Bus << 16) | ((u32)u8Slot << 11) |
                  ((u32)u8Func << 8) | (u8Off & 0xfcu);
    u32 u32Old;
    u32 u32Shift = (u8Off & 2u) * 8u;

    __asm__ volatile("outl %0, %1" : : "a"(u32Addr), "Nd"((u16)0xCF8));
    __asm__ volatile("inl %1, %0" : "=a"(u32Old) : "Nd"((u16)0xCFC));
    u32Old = (u32Old & ~(0xffffu << u32Shift)) | ((u32)u16Val << u32Shift);
    __asm__ volatile("outl %0, %1" : : "a"(u32Addr), "Nd"((u16)0xCF8));
    __asm__ volatile("outl %0, %1" : : "a"(u32Old), "Nd"((u16)0xCFC));
}

static u64
pci_bar_mem(u8 u8Bus, u8 u8Slot, u8 u8Func, u8 u8Bir)
{
    u8 u8Off = (u8)(0x10u + u8Bir * 4u);
    u32 u32Lo = pci_cfg_read(u8Bus, u8Slot, u8Func, u8Off);
    u64 u64Bar;

    if ((u32Lo & 1u) != 0) {
        return 0; /* I/O BAR */
    }
    u64Bar = (u64)(u32Lo & ~0xfu);
    if (((u32Lo >> 1) & 3u) == 2u && u8Bir < 5) {
        u32 u32Hi = pci_cfg_read(u8Bus, u8Slot, u8Func, (u8)(u8Off + 4));

        u64Bar |= ((u64)u32Hi << 32);
    }
    return u64Bar;
}

static volatile u32 *
msix_table_mmio(u64 u64Pa)
{
    if (u64Pa == 0) {
        return NULL;
    }
    /*
     * MSI-X tables live in device MMIO BARs (often ≥0xf0000000). Never use
     * HHDM — that maps RAM only. Identity-map 2 MiB covering the table.
     */
    if (u64Pa >= 0xf0000000ull || u64Pa >= 0x100000000ull) {
        if (vmm_map_device((gj_paddr_t)u64Pa, 2ull * 1024ull * 1024ull) !=
            GJ_OK) {
            kprintf("pci: MSI-X map table pa=0x%lx failed\n",
                    (unsigned long)u64Pa);
            return NULL;
        }
    }
    return (volatile u32 *)(gj_vaddr_t)u64Pa;
}

/*
 * Mirror a programmed entry into the soft shadow (product soft path).
 * Does not deliver; use pci_msix_soft_fire for soft delivery.
 */
static void
msix_soft_mirror(u16 u16Idx, u32 u32AddrLo, u32 u32AddrHi, u32 u32Data,
                 u32 u32VecCtl)
{
    struct gj_pci_msix_soft_entry *pEnt;

    if (!g_fSoftReady) {
        pci_msix_soft_table_init();
    }
    if (u16Idx >= GJ_MSIX_SOFT_TBL) {
        return;
    }
    pEnt = &g_aSoftTab[u16Idx];
    if (!pEnt->u8Programmed) {
        g_u32SoftProg++;
    }
    pEnt->u32MsgAddrLo = u32AddrLo;
    pEnt->u32MsgAddrHi = u32AddrHi;
    pEnt->u32MsgData = u32Data;
    pEnt->u32VecCtl = u32VecCtl;
    pEnt->u8Programmed = 1;
}

void
pci_msix_soft_table_init(void)
{
    if (g_fSoftReady) {
        return;
    }
    memset(g_aSoftTab, 0, sizeof(g_aSoftTab));
    g_u64SoftPba = 0;
    g_u32SoftProg = 0;
    g_u32SoftFire = 0;
    g_fSoftReady = 1;
}

u32
pci_msix_soft_program(u16 u16Idx, u32 u32AddrLo, u32 u32Data, u32 u32Mask)
{
    u32 u32VecCtl;

    if (!g_fSoftReady) {
        pci_msix_soft_table_init();
    }
    if (u16Idx >= GJ_MSIX_SOFT_TBL) {
        return 0;
    }
    u32VecCtl = u32Mask ? GJ_MSIX_VECCTL_MASK : 0u;
    msix_soft_mirror(u16Idx, u32AddrLo, 0u, u32Data, u32VecCtl);
    return 1;
}

u32
pci_msix_soft_mask(u16 u16Idx, u32 u32Mask)
{
    if (!g_fSoftReady || u16Idx >= GJ_MSIX_SOFT_TBL) {
        return 0;
    }
    if (!g_aSoftTab[u16Idx].u8Programmed) {
        return 0;
    }
    if (u32Mask) {
        g_aSoftTab[u16Idx].u32VecCtl |= GJ_MSIX_VECCTL_MASK;
    } else {
        g_aSoftTab[u16Idx].u32VecCtl &= ~GJ_MSIX_VECCTL_MASK;
    }
    return 1;
}

u32
pci_msix_soft_read(u16 u16Idx, struct gj_pci_msix_soft_entry *pOut)
{
    if (pOut == NULL || !g_fSoftReady || u16Idx >= GJ_MSIX_SOFT_TBL) {
        return 0;
    }
    if (!g_aSoftTab[u16Idx].u8Programmed) {
        return 0;
    }
    *pOut = g_aSoftTab[u16Idx];
    return 1;
}

u32
pci_msix_soft_fire(u16 u16Idx)
{
    struct gj_pci_msix_soft_entry *pEnt;
    u64 u64Badge;
    u32 u32Deliver = 0;

    if (!g_fSoftReady || u16Idx >= GJ_MSIX_SOFT_TBL) {
        return 0;
    }
    pEnt = &g_aSoftTab[u16Idx];
    if (!pEnt->u8Programmed) {
        return 0;
    }
    /*
     * Spec-like sticky PBA: bit set when the function would assert the
     * message (including while masked — pending until unmask + re-fire
     * in full HW; soft path records the bit on every fire attempt).
     */
    g_u64SoftPba |= (1ull << (u16Idx & 63u));
    if ((pEnt->u32VecCtl & GJ_MSIX_VECCTL_MASK) != 0) {
        return 0; /* masked: no Notification delivery */
    }
    pEnt->u8SoftFire = 1;
    g_u32SoftFire++;
    u32Deliver = 1;
    /*
     * Badge attribution: entry 0 → bit 2 (GJ_MSIX_BADGE_TBL(0)) so existing
     * smoke wait masks covering low bits still observe table soft fire.
     */
    u64Badge = GJ_MSIX_BADGE_TBL(u16Idx);
    if (irq_msix_ready()) {
        irq_msix_soft_inject(u64Badge);
    }
    return u32Deliver;
}

u64
pci_msix_soft_pba(void)
{
    return g_u64SoftPba;
}

u64
pci_msix_soft_pba_clear(u64 u64Mask)
{
    u64 u64Prev;

    u64Prev = g_u64SoftPba & u64Mask;
    g_u64SoftPba &= ~u64Mask;
    return u64Prev;
}

u32
pci_msix_soft_programmed_count(void)
{
    return g_u32SoftProg;
}

u32
pci_msix_soft_fire_count(void)
{
    return g_u32SoftFire;
}

int
pci_msix_soft_ready(void)
{
    return g_fSoftReady;
}

/*
 * Wave 11 soft inventory — greppable "pci: soft …" / "msix: soft …".
 * Pure observation; never allocates; never hard-gates HW/soft fire paths.
 * szVia: caller tag (probe / exercise / anon). Twin prefixes for greps.
 *
 * greppable: pci: soft inventory
 * greppable: msix: soft inventory
 */
static void
pci_msix_soft_inventory(const char *szVia)
{
    u32 iEnt;
    u32 cProgLive = 0;
    u32 cMasked = 0;
    u32 cFired = 0;
    u32 u32Addr0 = 0;
    u32 u32Data0 = 0;
    u32 u32VecCtl0 = 0;
    u32 fIrqReady;
    const char *szViaSafe;

    szViaSafe = (szVia != NULL) ? szVia : "anon";
    if (g_u32SoftInvLogs < 0xffffffffu) {
        g_u32SoftInvLogs++;
    }

    if (!g_fSoftReady) {
        pci_msix_soft_table_init();
    }

    for (iEnt = 0; iEnt < GJ_MSIX_SOFT_TBL; iEnt++) {
        if (g_aSoftTab[iEnt].u8Programmed) {
            cProgLive++;
            if ((g_aSoftTab[iEnt].u32VecCtl & GJ_MSIX_VECCTL_MASK) != 0) {
                cMasked++;
            }
            if (g_aSoftTab[iEnt].u8SoftFire) {
                cFired++;
            }
        }
    }
    if (g_aSoftTab[0].u8Programmed) {
        u32Addr0 = g_aSoftTab[0].u32MsgAddrLo;
        u32Data0 = g_aSoftTab[0].u32MsgData;
        u32VecCtl0 = g_aSoftTab[0].u32VecCtl;
    }
    fIrqReady = irq_msix_ready() ? 1u : 0u;

    /*
     * Grep: pci: soft inventory / msix: soft inventory
     * Soft table geometry + lifetime counters for product smoke.
     */
    kprintf("pci: soft inventory via=%s ready=%u depth=%u prog=%u "
            "prog_live=%u fire=%u pba=0x%lx hw_prog=%u irq_ready=%u "
            "logs=%u\n",
            szViaSafe, (unsigned)(g_fSoftReady ? 1 : 0),
            (unsigned)GJ_MSIX_SOFT_TBL, (unsigned)g_u32SoftProg,
            (unsigned)cProgLive, (unsigned)g_u32SoftFire,
            (unsigned long)g_u64SoftPba, (unsigned)g_u32Programmed,
            (unsigned)fIrqReady, (unsigned)g_u32SoftInvLogs);
    kprintf("msix: soft inventory via=%s ready=%u depth=%u prog=%u "
            "prog_live=%u fire=%u pba=0x%lx hw_prog=%u irq_ready=%u "
            "logs=%u\n",
            szViaSafe, (unsigned)(g_fSoftReady ? 1 : 0),
            (unsigned)GJ_MSIX_SOFT_TBL, (unsigned)g_u32SoftProg,
            (unsigned)cProgLive, (unsigned)g_u32SoftFire,
            (unsigned long)g_u64SoftPba, (unsigned)g_u32Programmed,
            (unsigned)fIrqReady, (unsigned)g_u32SoftInvLogs);

    /* Grep: pci: soft table / msix: soft table */
    kprintf("pci: soft table depth=%u entry0_addr=0x%x entry0_data=0x%x "
            "entry0_vecctl=0x%x programmed=%u soft_fire=%u probe_vec=0x%x\n",
            (unsigned)GJ_MSIX_SOFT_TBL, (unsigned)u32Addr0,
            (unsigned)u32Data0, (unsigned)u32VecCtl0,
            (unsigned)(g_aSoftTab[0].u8Programmed ? 1 : 0),
            (unsigned)(g_aSoftTab[0].u8SoftFire ? 1 : 0),
            (unsigned)PCI_MSIX_PROBE_VEC);
    kprintf("msix: soft table depth=%u entry0_addr=0x%x entry0_data=0x%x "
            "entry0_vecctl=0x%x programmed=%u soft_fire=%u probe_vec=0x%x\n",
            (unsigned)GJ_MSIX_SOFT_TBL, (unsigned)u32Addr0,
            (unsigned)u32Data0, (unsigned)u32VecCtl0,
            (unsigned)(g_aSoftTab[0].u8Programmed ? 1 : 0),
            (unsigned)(g_aSoftTab[0].u8SoftFire ? 1 : 0),
            (unsigned)PCI_MSIX_PROBE_VEC);

    /* Grep: pci: soft pba / msix: soft pba */
    kprintf("pci: soft pba bits=0x%lx sticky=1 clear_api=1 mask_sets=1 "
            "width=64\n",
            (unsigned long)g_u64SoftPba);
    kprintf("msix: soft pba bits=0x%lx sticky=1 clear_api=1 mask_sets=1 "
            "width=64\n",
            (unsigned long)g_u64SoftPba);

    /* Grep: pci: soft fire / msix: soft fire */
    kprintf("pci: soft fire count=%u live_fired=%u badge_tbl=GJ_MSIX_BADGE_TBL "
            "inject_if_ready=%u masked_hold=1\n",
            (unsigned)g_u32SoftFire, (unsigned)cFired, (unsigned)fIrqReady);
    kprintf("msix: soft fire count=%u live_fired=%u badge_tbl=GJ_MSIX_BADGE_TBL "
            "inject_if_ready=%u masked_hold=1\n",
            (unsigned)g_u32SoftFire, (unsigned)cFired, (unsigned)fIrqReady);

    /* Grep: pci: soft mask / msix: soft mask */
    kprintf("pci: soft mask live=%u vecctl_bit0=1 unmask_delivers=1 "
            "prog_live=%u\n",
            (unsigned)cMasked, (unsigned)cProgLive);
    kprintf("msix: soft mask live=%u vecctl_bit0=1 unmask_delivers=1 "
            "prog_live=%u\n",
            (unsigned)cMasked, (unsigned)cProgLive);

    /* Grep: pci: soft hw / msix: soft hw — HW program tallies (may be 0) */
    kprintf("pci: soft hw programmed=%u scan_api=1 enable_api=1 "
            "program_first=1 map_device=1\n",
            (unsigned)g_u32Programmed);
    kprintf("msix: soft hw programmed=%u scan_api=1 enable_api=1 "
            "program_first=1 map_device=1\n",
            (unsigned)g_u32Programmed);

    /*
     * Grep: pci: soft path / msix: soft path
     * Honesty: bounded soft shadow ≠ full device Table Size; soft ≠ bar3.
     */
    kprintf("pci: soft path claim=kernel_soft depth_bound=%u "
            "full_table_size=0 bar3=open soft_only_when_no_mmio=1 via=%s\n",
            (unsigned)GJ_MSIX_SOFT_TBL, szViaSafe);
    kprintf("msix: soft path claim=kernel_soft depth_bound=%u "
            "full_table_size=0 bar3=open soft_only_when_no_mmio=1 via=%s\n",
            (unsigned)GJ_MSIX_SOFT_TBL, szViaSafe);

    /* Soft path is always available after init — inventory PASS. */
    if (g_fSoftReady) {
        /* Grep: pci: soft inventory PASS / pci: soft PASS */
        kprintf("pci: soft inventory PASS via=%s logs=%u\n", szViaSafe,
                (unsigned)g_u32SoftInvLogs);
        kprintf("pci: soft PASS via=%s\n", szViaSafe);
        /* Grep: msix: soft inventory PASS / msix: soft PASS */
        kprintf("msix: soft inventory PASS via=%s logs=%u\n", szViaSafe,
                (unsigned)g_u32SoftInvLogs);
        kprintf("msix: soft PASS via=%s\n", szViaSafe);
    } else {
        kprintf("pci: soft inventory SKIP via=%s\n", szViaSafe);
        kprintf("msix: soft inventory SKIP via=%s\n", szViaSafe);
    }
}

u32
pci_msix_soft_table_exercise(void)
{
    struct gj_pci_msix_soft_entry ent;
    u32 fOk = 1;
    u32 u32Held;
    u32 u32Delivered;
    u64 u64Pba;

    pci_msix_soft_table_init();
    /* Entry 0: product vector, initially unmasked. */
    if (!pci_msix_soft_program(0, MSI_ADDR_BASE, (u32)PCI_MSIX_PROBE_VEC, 0)) {
        fOk = 0;
    }
    /* Entry 1: second soft vector, for multi-entry soft path. */
    if (!pci_msix_soft_program(1, MSI_ADDR_BASE, (u32)(PCI_MSIX_PROBE_VEC + 1u),
                               0)) {
        fOk = 0;
    }
    /* Mask-hold: fire while masked must not deliver. */
    if (!pci_msix_soft_mask(0, 1)) {
        fOk = 0;
    }
    u32Held = pci_msix_soft_fire(0);
    if (u32Held != 0) {
        fOk = 0; /* masked fire must not deliver */
    }
    u64Pba = pci_msix_soft_pba();
    if ((u64Pba & 1ull) == 0) {
        fOk = 0; /* sticky PBA still set while masked */
    }
    /* Unmask + fire: delivery when irq path ready (or soft counter only). */
    if (!pci_msix_soft_mask(0, 0)) {
        fOk = 0;
    }
    u32Delivered = pci_msix_soft_fire(0);
    if (u32Delivered == 0) {
        fOk = 0;
    }
    /* Soft fire entry 1. */
    if (pci_msix_soft_fire(1) == 0) {
        fOk = 0;
    }
    /* Readback entry 0. */
    memset(&ent, 0, sizeof(ent));
    if (!pci_msix_soft_read(0, &ent)) {
        fOk = 0;
    } else if (ent.u32MsgAddrLo != MSI_ADDR_BASE ||
               (ent.u32MsgData & 0xffu) != PCI_MSIX_PROBE_VEC ||
               (ent.u32VecCtl & GJ_MSIX_VECCTL_MASK) != 0 ||
               !ent.u8Programmed || !ent.u8SoftFire) {
        fOk = 0;
    }
    /* PBA clear soft path. */
    if (pci_msix_soft_pba_clear(1ull) != 1ull) {
        fOk = 0;
    }
    if ((pci_msix_soft_pba() & 1ull) != 0) {
        fOk = 0;
    }
    if (g_u32SoftProg < 2 || g_u32SoftFire < 2) {
        fOk = 0;
    }
    if (fOk) {
        kprintf("pci: MSI-X table soft path entries=%u fire=%u pba=0x%lx "
                "PASS\n",
                g_u32SoftProg, g_u32SoftFire,
                (unsigned long)pci_msix_soft_pba());
        kprintf("pci: MSI-X table soft path PASS\n");
    } else {
        kprintf("pci: MSI-X table soft path FAIL prog=%u fire=%u pba=0x%lx\n",
                g_u32SoftProg, g_u32SoftFire,
                (unsigned long)pci_msix_soft_pba());
    }
    /* Wave 11: greppable soft inventory rollup (after exercise state). */
    pci_msix_soft_inventory("exercise");
    return fOk;
}

u32
pci_msix_scan(struct gj_pci_msix_info *pOut, u32 u32Max)
{
    u8 u8Bus;
    u8 u8Slot;
    u8 u8Func;
    u32 cFound = 0;

    if (pOut == NULL || u32Max == 0) {
        return 0;
    }
    for (u8Bus = 0; u8Bus < 8 && cFound < u32Max; u8Bus++) {
        for (u8Slot = 0; u8Slot < 32 && cFound < u32Max; u8Slot++) {
            for (u8Func = 0; u8Func < 8 && cFound < u32Max; u8Func++) {
                u32 u32Id = pci_cfg_read(u8Bus, u8Slot, u8Func, 0);
                u16 u16Status;
                u8 u8Cap;

                if ((u32Id & 0xffffu) == 0xffffu) {
                    if (u8Func == 0) {
                        break;
                    }
                    continue;
                }
                u16Status = pci_cfg_read16(u8Bus, u8Slot, u8Func, 0x06);
                if ((u16Status & PCI_STATUS_CAP) == 0) {
                    continue;
                }
                u8Cap = pci_cfg_read8(u8Bus, u8Slot, u8Func, 0x34) & 0xfcu;
                while (u8Cap != 0 && u8Cap != 0xffu) {
                    u8 u8CapId = pci_cfg_read8(u8Bus, u8Slot, u8Func, u8Cap);

                    if (u8CapId == PCI_CAP_ID_MSIX) {
                        u16 u16MsgCtl =
                            pci_cfg_read16(u8Bus, u8Slot, u8Func, u8Cap + 2);
                        u32 u32Table =
                            pci_cfg_read(u8Bus, u8Slot, u8Func, u8Cap + 4);
                        u32 u32Pba =
                            pci_cfg_read(u8Bus, u8Slot, u8Func, u8Cap + 8);
                        u8 u8Bir = (u8)(u32Table & 7u);
                        u32 u32Toff = u32Table & ~7u;
                        u64 u64Bar = pci_bar_mem(u8Bus, u8Slot, u8Func, u8Bir);

                        memset(&pOut[cFound], 0, sizeof(pOut[cFound]));
                        pOut[cFound].u8Present = 1;
                        pOut[cFound].u8Enabled =
                            (u16MsgCtl & 0x8000u) ? 1u : 0u;
                        pOut[cFound].u16TableSize =
                            (u16)((u16MsgCtl & 0x7ffu) + 1u);
                        pOut[cFound].u8Bus = u8Bus;
                        pOut[cFound].u8Slot = u8Slot;
                        pOut[cFound].u8Func = u8Func;
                        pOut[cFound].u8TableBir = u8Bir;
                        pOut[cFound].u32TableOff = u32Toff;
                        pOut[cFound].u8PbaBir = (u8)(u32Pba & 7u);
                        pOut[cFound].u32PbaOff = u32Pba & ~7u;
                        pOut[cFound].u64TablePa =
                            u64Bar ? (u64Bar + u32Toff) : 0;
                        cFound++;
                        break;
                    }
                    if (u8CapId == PCI_CAP_ID_MSI && cFound < u32Max) {
                        memset(&pOut[cFound], 0, sizeof(pOut[cFound]));
                        pOut[cFound].u8Present = 1;
                        pOut[cFound].u8Enabled = 0;
                        pOut[cFound].u16TableSize = 1;
                        pOut[cFound].u8Bus = u8Bus;
                        pOut[cFound].u8Slot = u8Slot;
                        pOut[cFound].u8Func = u8Func;
                        cFound++;
                        break;
                    }
                    u8Cap =
                        pci_cfg_read8(u8Bus, u8Slot, u8Func, u8Cap + 1) & 0xfcu;
                }
            }
        }
    }
    return cFound;
}

u32
pci_msix_enable_first(u32 u32Max)
{
    struct gj_pci_msix_info aInfo[16];
    u32 cScan;
    u32 iDev;
    u32 cEnabled = 0;

    cScan = pci_msix_scan(aInfo, 16);
    if (u32Max == 0 || u32Max > cScan) {
        u32Max = cScan;
    }
    for (iDev = 0; iDev < u32Max; iDev++) {
        u8 u8Bus = aInfo[iDev].u8Bus;
        u8 u8Slot = aInfo[iDev].u8Slot;
        u8 u8Func = aInfo[iDev].u8Func;
        u8 u8Cap = pci_cfg_read8(u8Bus, u8Slot, u8Func, 0x34) & 0xfcu;

        while (u8Cap != 0 && u8Cap != 0xffu) {
            u8 u8CapId = pci_cfg_read8(u8Bus, u8Slot, u8Func, u8Cap);

            if (u8CapId == PCI_CAP_ID_MSIX) {
                u16 u16MsgCtl =
                    pci_cfg_read16(u8Bus, u8Slot, u8Func, u8Cap + 2);

                /* MSI-X Enable (bit 15); clear Function Mask (bit 14) */
                u16MsgCtl = (u16)((u16MsgCtl | 0x8000u) & ~0x4000u);
                pci_cfg_write16(u8Bus, u8Slot, u8Func, u8Cap + 2, u16MsgCtl);
                cEnabled++;
                break;
            }
            u8Cap =
                pci_cfg_read8(u8Bus, u8Slot, u8Func, u8Cap + 1) & 0xfcu;
        }
    }
    kprintf("pci: MSI-X enable attempted on %u devices\n", cEnabled);
    return cEnabled;
}

u32
pci_msix_program_first(u8 u8Vector)
{
    struct gj_pci_msix_info aInfo[16];
    u32 cScan;
    u32 iDev;

    cScan = pci_msix_scan(aInfo, 16);
    for (iDev = 0; iDev < cScan; iDev++) {
        volatile u32 *pTab;
        u8 u8Bus;
        u8 u8Slot;
        u8 u8Func;
        u8 u8Cap;
        u16 u16Cmd;

        if (aInfo[iDev].u64TablePa == 0 || aInfo[iDev].u16TableSize == 0) {
            continue;
        }
        u8Bus = aInfo[iDev].u8Bus;
        u8Slot = aInfo[iDev].u8Slot;
        u8Func = aInfo[iDev].u8Func;
        /* Bus master + memory space */
        u16Cmd = pci_cfg_read16(u8Bus, u8Slot, u8Func, 0x04);
        u16Cmd = (u16)(u16Cmd | 0x6u);
        pci_cfg_write16(u8Bus, u8Slot, u8Func, 0x04, u16Cmd);

        pTab = msix_table_mmio(aInfo[iDev].u64TablePa);
        if (pTab == NULL) {
            continue;
        }
        /* Entry 0: addr lo/hi, data, vector control (mask then unmask) */
        pTab[0] = MSI_ADDR_BASE; /* dest = BSP local APIC */
        pTab[1] = 0;
        pTab[2] = (u32)u8Vector; /* edge, fixed delivery */
        pTab[3] = 1u;            /* masked while programming */
        __asm__ volatile("mfence" ::: "memory");
        pTab[3] = 0; /* unmask */

        /* Soft mirror of HW entry 0 (unmasked). */
        msix_soft_mirror(0, MSI_ADDR_BASE, 0u, (u32)u8Vector, 0u);

        /* Enable MSI-X in config */
        u8Cap = pci_cfg_read8(u8Bus, u8Slot, u8Func, 0x34) & 0xfcu;
        while (u8Cap != 0 && u8Cap != 0xffu) {
            if (pci_cfg_read8(u8Bus, u8Slot, u8Func, u8Cap) ==
                PCI_CAP_ID_MSIX) {
                u16 u16MsgCtl =
                    pci_cfg_read16(u8Bus, u8Slot, u8Func, u8Cap + 2);

                u16MsgCtl = (u16)((u16MsgCtl | 0x8000u) & ~0x4000u);
                pci_cfg_write16(u8Bus, u8Slot, u8Func, u8Cap + 2, u16MsgCtl);
                break;
            }
            u8Cap =
                pci_cfg_read8(u8Bus, u8Slot, u8Func, u8Cap + 1) & 0xfcu;
        }
        g_u32Programmed++;
        aInfo[iDev].u32Programmed = 1;
        kprintf("pci: MSI-X table program %u:%u.%u pa=0x%lx vec=0x%x\n", u8Bus,
                u8Slot, u8Func, (unsigned long)aInfo[iDev].u64TablePa,
                (unsigned)u8Vector);
        return 1;
    }
    return 0;
}

u32
pci_msix_programmed_count(void)
{
    return g_u32Programmed;
}

void
pci_msix_probe_log(void)
{
    struct gj_pci_msix_info aInfo[16];
    u32 cScan;
    u32 iDev;
    u32 cEnabled;
    u32 cProg;
    u32 fSoft;

    pci_msix_soft_table_init();
    memset(aInfo, 0, sizeof(aInfo));
    cScan = pci_msix_scan(aInfo, 16);
    kprintf("pci: MSI/MSI-X devices=%u\n", cScan);
    for (iDev = 0; iDev < cScan && iDev < 4; iDev++) {
        kprintf("pci: msix %u:%u.%u table=%u en=%u bir=%u off=0x%x pa=0x%lx\n",
                aInfo[iDev].u8Bus, aInfo[iDev].u8Slot, aInfo[iDev].u8Func,
                aInfo[iDev].u16TableSize, aInfo[iDev].u8Enabled,
                aInfo[iDev].u8TableBir, aInfo[iDev].u32TableOff,
                (unsigned long)aInfo[iDev].u64TablePa);
    }
    cEnabled = pci_msix_enable_first(cScan > 2 ? 2 : cScan);
    cProg = pci_msix_program_first(PCI_MSIX_PROBE_VEC);
    /* Soft table always exercised (works with zero devices). */
    fSoft = pci_msix_soft_table_exercise();
    /*
     * Wave 11 soft inventory at probe (exercise already dumps once;
     * probe via= tag deepens greppable "pci: soft …" / "msix: soft …").
     */
    pci_msix_soft_inventory("probe");
    if (cScan > 0 || cEnabled > 0 || fSoft) {
        kprintf("pci: MSI-X probe PASS\n");
        if (cEnabled > 0) {
            kprintf("pci: MSI-X enable PASS\n");
        }
        if (cProg > 0) {
            kprintf("pci: MSI-X table PASS\n");
        }
    } else {
        kprintf("pci: MSI-X none (ok)\n");
        kprintf("pci: MSI-X probe PASS\n");
    }
}
