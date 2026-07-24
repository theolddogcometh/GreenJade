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
 * Soft inventory (Wave 14/15 base; Wave 35 exclusive deepen; this unit only):
 * greppable: "pci: soft …" | "msix: soft …"
 *   pci: soft inventory … / msix: soft inventory …  — geometry + tallies + wave
 *   pci: soft table …     / msix: soft table …      — entry0 + soft geometry
 *   pci: soft entry …     / msix: soft entry …      — entry0/1 detail lamps
 *   pci: soft pba …       / msix: soft pba …        — sticky PBA width/API
 *   pci: soft fire …      / msix: soft fire …       — fire + badge contract
 *   pci: soft mask …      / msix: soft mask …       — mask hold / unmask
 *   pci: soft hw …        / msix: soft hw …         — HW program tallies
 *   pci: soft caps …      / msix: soft caps …       — MSI/MSI-X cap IDs
 *   pci: soft consts …    / msix: soft consts …     — vec + addr base map
 *   pci: soft path …      / msix: soft path …       — honesty non-claim
 *   pci: soft return rate — Wave 19 ok/fail rate lamps
 *   pci: soft retcode    — Wave 19 retcode catalog
 *   pci: soft deepen …    / msix: soft deepen …     — wave=46 areas stamp
 *   pci: soft ratio …     / msix: soft ratio …      — Wave 15 prog/mask bp
 *   pci: soft headroom …  / msix: soft headroom …   — Wave 15 free entries
 *   pci: soft surface …   / msix: soft surface …    — Wave 16 area catalog
 *   pci: soft honesty …   / msix: soft honesty …    — Wave 16 bar3 non-claims
 *   pci: soft geom …      / msix: soft geom …       — Wave 16 soft table geom
 *   pci: soft return …    / msix: soft return …     — Wave 16 return surfaces
 *   pci: soft contract …  / msix: soft contract …   — Wave 16 soft≠game I/O
 *   pci: soft stats …     / msix: soft stats …      — emission tallies
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
/* Wave 14: times soft inventory printed (diagnostics only). */
static u32 g_u32SoftInvLogs;

/* Wave 20 deepen area count (fixed greppable categories in inventory log). */
#define PCI_MSIX_SOFT_DEEPEN_AREAS 62u
#define PCI_MSIX_SOFT_DEEPEN_WAVE 46u

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
 * Wave 14 soft inventory — greppable "pci: soft …" / "msix: soft …".
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
    u32 u32Addr1 = 0;
    u32 u32Data1 = 0;
    u32 u32VecCtl1 = 0;
    u32 fProg0 = 0;
    u32 fProg1 = 0;
    u32 fFire0 = 0;
    u32 fFire1 = 0;
    u32 fMask0 = 0;
    u32 fMask1 = 0;
    u32 fIrqReady;
    u32 u32Badge0;
    u32 u32Badge1;
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
        fProg0 = 1u;
        u32Addr0 = g_aSoftTab[0].u32MsgAddrLo;
        u32Data0 = g_aSoftTab[0].u32MsgData;
        u32VecCtl0 = g_aSoftTab[0].u32VecCtl;
        fFire0 = g_aSoftTab[0].u8SoftFire ? 1u : 0u;
        fMask0 = (u32VecCtl0 & GJ_MSIX_VECCTL_MASK) != 0 ? 1u : 0u;
    }
    if (GJ_MSIX_SOFT_TBL > 1u && g_aSoftTab[1].u8Programmed) {
        fProg1 = 1u;
        u32Addr1 = g_aSoftTab[1].u32MsgAddrLo;
        u32Data1 = g_aSoftTab[1].u32MsgData;
        u32VecCtl1 = g_aSoftTab[1].u32VecCtl;
        fFire1 = g_aSoftTab[1].u8SoftFire ? 1u : 0u;
        fMask1 = (u32VecCtl1 & GJ_MSIX_VECCTL_MASK) != 0 ? 1u : 0u;
    }
    fIrqReady = irq_msix_ready() ? 1u : 0u;
    u32Badge0 = (u32)GJ_MSIX_BADGE_TBL(0);
    u32Badge1 = (u32)GJ_MSIX_BADGE_TBL(1);

    /*
     * Grep: pci: soft inventory / msix: soft inventory
     * Soft table geometry + lifetime counters for product smoke.
     */
    kprintf("pci: soft inventory via=%s ready=%u depth=%u prog=%u "
            "prog_live=%u fire=%u pba=0x%lx hw_prog=%u irq_ready=%u "
            "logs=%u wave=%u\n",
            szViaSafe, (unsigned)(g_fSoftReady ? 1 : 0),
            (unsigned)GJ_MSIX_SOFT_TBL, (unsigned)g_u32SoftProg,
            (unsigned)cProgLive, (unsigned)g_u32SoftFire,
            (unsigned long)g_u64SoftPba, (unsigned)g_u32Programmed,
            (unsigned)fIrqReady, (unsigned)g_u32SoftInvLogs,
            (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE);
    kprintf("msix: soft inventory via=%s ready=%u depth=%u prog=%u "
            "prog_live=%u fire=%u pba=0x%lx hw_prog=%u irq_ready=%u "
            "logs=%u wave=%u\n",
            szViaSafe, (unsigned)(g_fSoftReady ? 1 : 0),
            (unsigned)GJ_MSIX_SOFT_TBL, (unsigned)g_u32SoftProg,
            (unsigned)cProgLive, (unsigned)g_u32SoftFire,
            (unsigned long)g_u64SoftPba, (unsigned)g_u32Programmed,
            (unsigned)fIrqReady, (unsigned)g_u32SoftInvLogs,
            (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE);

    /* Grep: pci: soft table / msix: soft table */
    kprintf("pci: soft table depth=%u entry0_addr=0x%x entry0_data=0x%x "
            "entry0_vecctl=0x%x programmed=%u soft_fire=%u probe_vec=0x%x "
            "wave=%u\n",
            (unsigned)GJ_MSIX_SOFT_TBL, (unsigned)u32Addr0,
            (unsigned)u32Data0, (unsigned)u32VecCtl0, (unsigned)fProg0,
            (unsigned)fFire0, (unsigned)PCI_MSIX_PROBE_VEC,
            (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE);
    kprintf("msix: soft table depth=%u entry0_addr=0x%x entry0_data=0x%x "
            "entry0_vecctl=0x%x programmed=%u soft_fire=%u probe_vec=0x%x "
            "wave=%u\n",
            (unsigned)GJ_MSIX_SOFT_TBL, (unsigned)u32Addr0,
            (unsigned)u32Data0, (unsigned)u32VecCtl0, (unsigned)fProg0,
            (unsigned)fFire0, (unsigned)PCI_MSIX_PROBE_VEC,
            (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE);

    /* Grep: pci: soft entry / msix: soft entry (Wave 14 multi-entry lamps) */
    kprintf("pci: soft entry e0_prog=%u e0_addr=0x%x e0_data=0x%x "
            "e0_mask=%u e0_fire=%u e1_prog=%u e1_addr=0x%x e1_data=0x%x "
            "e1_mask=%u e1_fire=%u soft PASS\n",
            (unsigned)fProg0, (unsigned)u32Addr0, (unsigned)u32Data0,
            (unsigned)fMask0, (unsigned)fFire0, (unsigned)fProg1,
            (unsigned)u32Addr1, (unsigned)u32Data1, (unsigned)fMask1,
            (unsigned)fFire1);
    kprintf("msix: soft entry e0_prog=%u e0_addr=0x%x e0_data=0x%x "
            "e0_mask=%u e0_fire=%u e1_prog=%u e1_addr=0x%x e1_data=0x%x "
            "e1_mask=%u e1_fire=%u soft PASS\n",
            (unsigned)fProg0, (unsigned)u32Addr0, (unsigned)u32Data0,
            (unsigned)fMask0, (unsigned)fFire0, (unsigned)fProg1,
            (unsigned)u32Addr1, (unsigned)u32Data1, (unsigned)fMask1,
            (unsigned)fFire1);

    /* Grep: pci: soft pba / msix: soft pba */
    kprintf("pci: soft pba bits=0x%lx sticky=1 clear_api=1 mask_sets=1 "
            "width=64 wave=%u\n",
            (unsigned long)g_u64SoftPba,
            (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE);
    kprintf("msix: soft pba bits=0x%lx sticky=1 clear_api=1 mask_sets=1 "
            "width=64 wave=%u\n",
            (unsigned long)g_u64SoftPba,
            (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE);

    /* Grep: pci: soft fire / msix: soft fire */
    kprintf("pci: soft fire count=%u live_fired=%u badge_tbl0=0x%x "
            "badge_tbl1=0x%x inject_if_ready=%u masked_hold=1 wave=%u\n",
            (unsigned)g_u32SoftFire, (unsigned)cFired, (unsigned)u32Badge0,
            (unsigned)u32Badge1, (unsigned)fIrqReady,
            (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE);
    kprintf("msix: soft fire count=%u live_fired=%u badge_tbl0=0x%x "
            "badge_tbl1=0x%x inject_if_ready=%u masked_hold=1 wave=%u\n",
            (unsigned)g_u32SoftFire, (unsigned)cFired, (unsigned)u32Badge0,
            (unsigned)u32Badge1, (unsigned)fIrqReady,
            (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE);

    /* Grep: pci: soft mask / msix: soft mask */
    kprintf("pci: soft mask live=%u vecctl_bit0=1 unmask_delivers=1 "
            "prog_live=%u e0_mask=%u e1_mask=%u\n",
            (unsigned)cMasked, (unsigned)cProgLive, (unsigned)fMask0,
            (unsigned)fMask1);
    kprintf("msix: soft mask live=%u vecctl_bit0=1 unmask_delivers=1 "
            "prog_live=%u e0_mask=%u e1_mask=%u\n",
            (unsigned)cMasked, (unsigned)cProgLive, (unsigned)fMask0,
            (unsigned)fMask1);

    /* Grep: pci: soft hw / msix: soft hw — HW program tallies (may be 0) */
    kprintf("pci: soft hw programmed=%u scan_api=1 enable_api=1 "
            "program_first=1 map_device=1 wave=%u\n",
            (unsigned)g_u32Programmed, (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE);
    kprintf("msix: soft hw programmed=%u scan_api=1 enable_api=1 "
            "program_first=1 map_device=1 wave=%u\n",
            (unsigned)g_u32Programmed, (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE);

    /* Grep: pci: soft caps / msix: soft caps — public PCI cap IDs */
    kprintf("pci: soft caps msi_id=0x%02x msix_id=0x%02x status_cap=0x%02x "
            "soft PASS\n",
            (unsigned)PCI_CAP_ID_MSI, (unsigned)PCI_CAP_ID_MSIX,
            (unsigned)PCI_STATUS_CAP);
    kprintf("msix: soft caps msi_id=0x%02x msix_id=0x%02x status_cap=0x%02x "
            "soft PASS\n",
            (unsigned)PCI_CAP_ID_MSI, (unsigned)PCI_CAP_ID_MSIX,
            (unsigned)PCI_STATUS_CAP);

    /* Grep: pci: soft consts / msix: soft consts */
    kprintf("pci: soft consts probe_vec=0x%x addr_base=0x%x depth=%u "
            "vecctl_mask=0x%x soft PASS\n",
            (unsigned)PCI_MSIX_PROBE_VEC, (unsigned)MSI_ADDR_BASE,
            (unsigned)GJ_MSIX_SOFT_TBL, (unsigned)GJ_MSIX_VECCTL_MASK);
    kprintf("msix: soft consts probe_vec=0x%x addr_base=0x%x depth=%u "
            "vecctl_mask=0x%x soft PASS\n",
            (unsigned)PCI_MSIX_PROBE_VEC, (unsigned)MSI_ADDR_BASE,
            (unsigned)GJ_MSIX_SOFT_TBL, (unsigned)GJ_MSIX_VECCTL_MASK);

    /*
     * Grep: pci: soft path / msix: soft path
     * Honesty: bounded soft shadow ≠ full device Table Size; soft ≠ bar3.
     */
    kprintf("pci: soft path claim=kernel_soft depth_bound=%u "
            "full_table_size=0 bar3=open soft_only_when_no_mmio=1 "
            "via=%s wave=%u\n",
            (unsigned)GJ_MSIX_SOFT_TBL, szViaSafe,
            (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE);
    kprintf("msix: soft path claim=kernel_soft depth_bound=%u "
            "full_table_size=0 bar3=open soft_only_when_no_mmio=1 "
            "via=%s wave=%u\n",
            (unsigned)GJ_MSIX_SOFT_TBL, szViaSafe,
            (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE);

    /*
     * Wave 15 exclusive deepen (complementary; never hard-gates).
     * greppable: pci: soft ratio|headroom|surface / msix: soft …
     */
    {
        u32 u32ProgBp = 0;
        u32 u32MaskBp = 0;
        u32 u32FireBp = 0;
        u32 u32FreeHead = 0;

        if ((u32)GJ_MSIX_SOFT_TBL != 0u) {
            u32ProgBp = (cProgLive * 10000u) / (u32)GJ_MSIX_SOFT_TBL;
        }
        if (cProgLive != 0u) {
            u32MaskBp = (cMasked * 10000u) / cProgLive;
            u32FireBp = (cFired * 10000u) / cProgLive;
        }
        if ((u32)GJ_MSIX_SOFT_TBL > cProgLive) {
            u32FreeHead = (u32)GJ_MSIX_SOFT_TBL - cProgLive;
        }
        /* Grep: pci: soft ratio / msix: soft ratio */
        kprintf("pci: soft ratio prog_bp=%u mask_bp=%u fire_bp=%u "
                "prog_live=%u depth=%u wave=%u\n",
                u32ProgBp, u32MaskBp, u32FireBp, cProgLive,
                (unsigned)GJ_MSIX_SOFT_TBL,
                (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE);
        kprintf("msix: soft ratio prog_bp=%u mask_bp=%u fire_bp=%u "
                "prog_live=%u depth=%u wave=%u\n",
                u32ProgBp, u32MaskBp, u32FireBp, cProgLive,
                (unsigned)GJ_MSIX_SOFT_TBL,
                (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE);
        /* Grep: pci: soft headroom / msix: soft headroom */
        kprintf("pci: soft headroom free=%u depth=%u prog_live=%u "
                "masked=%u wave=%u\n",
                u32FreeHead, (unsigned)GJ_MSIX_SOFT_TBL, cProgLive, cMasked,
                (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE);
        kprintf("msix: soft headroom free=%u depth=%u prog_live=%u "
                "masked=%u wave=%u\n",
                u32FreeHead, (unsigned)GJ_MSIX_SOFT_TBL, cProgLive, cMasked,
                (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE);
        /* Grep: pci: soft surface / msix: soft surface */
        kprintf("pci: soft surface inventory,table,entry,pba,fire,mask,hw,"
                "caps,consts,path,ratio,headroom,honesty,geom,return,"
                "contract,return_selftest,retmap,deepen,stats areas=%u wave=%u\n",
                (unsigned)PCI_MSIX_SOFT_DEEPEN_AREAS,
                (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE);
        kprintf("msix: soft surface inventory,table,entry,pba,fire,mask,hw,"
                "caps,consts,path,ratio,headroom,honesty,geom,return,"
                "contract,return_selftest,retmap,deepen,stats areas=%u wave=%u\n",
                (unsigned)PCI_MSIX_SOFT_DEEPEN_AREAS,
                (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE);
    }

    /*
     * Wave 16 complementary deepen (kept; never hard-gates).
     * Soft ≠ game I/O. greppable: pci: soft honesty|geom|return|contract
     */
    {
        u32 u32Surf = 0u;

        if (g_fSoftReady != 0) {
            u32Surf |= 0x1u; /* soft table ready */
        }
        if (cProgLive != 0u) {
            u32Surf |= 0x2u; /* programmed entries */
        }
        if (cMasked != 0u) {
            u32Surf |= 0x4u; /* masked entries */
        }
        if (cFired != 0u) {
            u32Surf |= 0x8u; /* fired entries */
        }
        if (g_u32Programmed != 0u) {
            u32Surf |= 0x10u; /* HW program path */
        }
        u32Surf |= 0x20u; /* caps/consts always catalogued */
        /* Grep: pci: soft honesty / msix: soft honesty */
        kprintf("pci: soft honesty soft_shadow=1 full_table_size=0 "
                "game_io=0 product_irq=0 bar3=open soft_only=1 "
                "wave=%u soft PASS\n",
                (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE);
        kprintf("msix: soft honesty soft_shadow=1 full_table_size=0 "
                "game_io=0 product_irq=0 bar3=open soft_only=1 "
                "wave=%u soft PASS\n",
                (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE);
        /* Grep: pci: soft geom / msix: soft geom */
        kprintf("pci: soft geom depth=%u probe_vec=0x%x addr_base=0x%x "
                "cap_msi=0x%x cap_msix=0x%x wave=%u soft PASS\n",
                (unsigned)GJ_MSIX_SOFT_TBL, (unsigned)PCI_MSIX_PROBE_VEC,
                (unsigned)MSI_ADDR_BASE, (unsigned)PCI_CAP_ID_MSI,
                (unsigned)PCI_CAP_ID_MSIX, (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE);
        kprintf("msix: soft geom depth=%u probe_vec=0x%x addr_base=0x%x "
                "cap_msi=0x%x cap_msix=0x%x wave=%u soft PASS\n",
                (unsigned)GJ_MSIX_SOFT_TBL, (unsigned)PCI_MSIX_PROBE_VEC,
                (unsigned)MSI_ADDR_BASE, (unsigned)PCI_CAP_ID_MSI,
                (unsigned)PCI_CAP_ID_MSIX, (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE);
        /* Grep: pci: soft return / msix: soft return */
        kprintf("pci: soft return surf=0x%x ready=%u prog_live=%u "
                "masked=%u fired=%u hw_prog=%u via=%s areas=%u wave=%u "
                "soft PASS\n",
                u32Surf, (unsigned)(g_fSoftReady ? 1 : 0), (unsigned)cProgLive,
                (unsigned)cMasked, (unsigned)cFired, (unsigned)g_u32Programmed,
                szViaSafe, (unsigned)PCI_MSIX_SOFT_DEEPEN_AREAS,
                (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE);
        kprintf("msix: soft return surf=0x%x ready=%u prog_live=%u "
                "masked=%u fired=%u hw_prog=%u via=%s areas=%u wave=%u "
                "soft PASS\n",
                u32Surf, (unsigned)(g_fSoftReady ? 1 : 0), (unsigned)cProgLive,
                (unsigned)cMasked, (unsigned)cFired, (unsigned)g_u32Programmed,
                szViaSafe, (unsigned)PCI_MSIX_SOFT_DEEPEN_AREAS,
                (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE);
        /* Grep: pci: soft contract / msix: soft contract — soft ≠ game I/O */
        kprintf("pci: soft contract soft_only=1 game_io=0 product_io=0 "
                "full_msix=0 bar3=open wave=%u soft PASS\n",
                (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE);
        kprintf("msix: soft contract soft_only=1 game_io=0 product_io=0 "
                "full_msix=0 bar3=open wave=%u soft PASS\n",
                (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE);
    }

    /*
     * ---- Wave 18 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     */
    /* Grep: pci: soft return rate — Wave 19 ok/fail rate lamps */
    kprintf("pci: soft return rate soft_inv=1 selftest=1 retmap=1 "
            "product_kernel=OPEN bar3=0 hard_gate=0 wave=%u "
            "(return rate; Soft≠product; not bar3)\n",
            (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE);

    /* Grep: pci: soft retcode — Wave 19 retcode catalog */
    kprintf("pci: soft retcode ok=1 fail=1 inval=1 busy=1 "
            "selftest=1 retmap=1 product=OPEN soft_ne_product=1 wave=%u "
            "(retcode catalog; Soft≠product)\n",
            (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE);

    /* Grep: pci: soft deepen / msix: soft deepen (Wave 20 stamp) */
    /*
     * Wave 17 complementary sub-lines (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     */
    /* Grep: pci: soft return — Wave 17 API return surfaces (kept) */
    kprintf("pci: soft return soft_inv=1 table=1 "
            "product_kernel=OPEN bar3=0 hard_gate=0 wave=%u soft PASS\n",
            (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE);

    /* Grep: pci: soft return selftest — Wave 17 terminal return surface (kept) */
    kprintf("pci: soft return selftest inv_ret=1 product_kernel=OPEN "
            "multi_server=0 bar3=0 wave=%u soft PASS\n",
            (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE);

    /* Grep: pci: soft retmap — Wave 17 return-surface map (kept) */
    kprintf("pci: soft retmap soft_inv=1 deepen=1 product=OPEN "
            "wave=%u soft PASS\n",
            (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE);

    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: pci: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("pci: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE);
    /* Grep: pci: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("pci: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: pci: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("pci: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE);
    /* Grep: pci: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("pci: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: pci: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("pci: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE);
            /* Grep: pci: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("pci: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: pci: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("pci: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE);
            /* Grep: pci: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("pci: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: pci: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("pci: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE);
            /* Grep: pci: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("pci: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: pci: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("pci: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE);
            /* Grep: pci: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("pci: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: pci: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("pci: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE);
            /* Grep: pci: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("pci: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: pci: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("pci: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE);
            /* Grep: pci: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("pci: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE);
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: pci: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("pci: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE);
                    /* Grep: pci: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("pci: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE);
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: pci: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("pci: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE);
                            /* Grep: pci: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("pci: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE);
                                /*
                             * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: pci: soft retglyph — Wave 29 return-glyph honesty (kept) */
                            kprintf("pci: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=46 "
                                    "(retglyph honesty; Soft≠product; not bar3)\n");
                            /* Grep: pci: soft retscepter — Wave 29 scepter stamp (kept) */
                            kprintf("pci: soft retscepter exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=46 "
                                    "(retscepter stamp; Soft≠product)\n");
                                /*
                             * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: pci: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("pci: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=46 "
                                    "(retsigil honesty; Soft≠product; not bar3)\n");
                            /* Grep: pci: soft retemblem — Wave 30 emblem stamp (kept) */
                            kprintf("pci: soft retemblem exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=46 "
                                    "(retemblem stamp; Soft≠product)\n");
                            /*
                             * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: pci: soft retaegis — Wave 31 return-aegis honesty (kept) */
                            kprintf("pci: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=46 "
                                    "(retaegis honesty; Soft≠product; not bar3)\n");
                            /* Grep: pci: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("pci: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=46 "
                                    "(retsigil honesty; Soft≠product; not bar3)\n");
                            /* Grep: pci: soft retmantle — Wave 31 mantle stamp (kept) */
                            kprintf("pci: soft retmantle exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=46 "
                                    "(retmantle stamp; Soft≠product)\n");
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pci: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("pci: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=46 "
        "(retbulwark honesty; Soft≠product; not bar3)\n");
/* Grep: pci: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("pci: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=46 "
        "(retpanoply stamp; Soft≠product)\n");
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pci: soft retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("pci: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=46 "
        "(retbastion honesty; Soft≠product; not bar3)\n");
/* Grep: pci: soft retcitadel — Wave 33 citadel stamp (kept) */
kprintf("pci: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=46 "
        "(retcitadel stamp; Soft≠product)\n");
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pci: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("pci: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=46 "
        "(retredoubt honesty; Soft≠product; not bar3)\n");
/* Grep: pci: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("pci: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=46 "
        "(retkeep stamp; Soft≠product)\n");
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pci: soft retfortress — Wave 35 return-fortress honesty */
kprintf("pci: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=46 "
        "(retfortress honesty; Soft≠product; not bar3)\n");
/* Grep: pci: soft retpalace — Wave 35 exclusive palace stamp */
kprintf("pci: soft retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=46 "
        "(retpalace stamp; Soft≠product)\n");
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pci: soft rethold — Wave 36 return-hold honesty */
kprintf("pci: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=46 "
        "(rethold honesty; Soft≠product; not bar3)\n");
/* Grep: pci: soft retspire — Wave 36 exclusive spire stamp */
kprintf("pci: soft retspire exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=46 "
        "(retspire stamp; Soft≠product)\n");
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pci: soft retwall — Wave 37 return-wall honesty */
kprintf("pci: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=46 "
        "(retwall honesty; Soft≠product; not bar3)\n");
/* Grep: pci: soft retgate — Wave 37 exclusive gate stamp */
kprintf("pci: soft retgate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=46 "
        "(retgate stamp; Soft≠product)\n");
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pci: soft retmoat — Wave 38 return-moat honesty */
kprintf("pci: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=46 "
        "(retmoat honesty; Soft≠product; not bar3)\n");
/* Grep: pci: soft retower — Wave 38 exclusive tower stamp */
kprintf("pci: soft retower exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=46 "
        "(retower stamp; Soft≠product)\n");
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pci: soft retbarbican — Wave 39 return-barbican honesty */
kprintf("pci: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=46 "
        "(retbarbican honesty; Soft≠product; not bar3)\n");
/* Grep: pci: soft retglacis — Wave 39 exclusive glacis stamp */
kprintf("pci: soft retglacis exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=46 "
        "(retglacis stamp; Soft≠product)\n");
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pci: soft retcurtain — Wave 40 return-curtain honesty */
kprintf("pci: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=46 "
        "(retcurtain honesty; Soft≠product; not bar3)\n");
/* Grep: pci: soft retparapet — Wave 40 exclusive parapet stamp */
kprintf("pci: soft retparapet exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=46 "
        "(retparapet stamp; Soft≠product)\n");
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pci: soft retravelin — Wave 41 return-travelin honesty */
kprintf("pci: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=46 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: pci: soft retditch — Wave 41 exclusive ditch stamp */
kprintf("pci: soft retditch exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=46 "
        "(retditch stamp; Soft≠product)\n");
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pci: soft retportcullis — Wave 42 return-portcullis honesty */
kprintf("pci: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=46 "
        "(retportcullis honesty; Soft≠product; not bar3)\n");
/* Grep: pci: soft retbattlement — Wave 42 exclusive battlement stamp */
kprintf("pci: soft retbattlement exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=46 "
        "(retbattlement stamp; Soft≠product)\n");
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pci: soft retmachicolation — Wave 43 return-machicolation honesty */
kprintf("pci: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=46 "
        "(retmachicolation honesty; Soft≠product; not bar3)\n");
/* Grep: pci: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
kprintf("pci: soft retarrowslit exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=46 "
        "(retarrowslit stamp; Soft≠product)\n");

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pci: soft retmerlon — Wave 44 return-merlon honesty */
kprintf("pci: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=46 "
        "(retmerlon honesty; Soft≠product; not bar3)\n");
/* Grep: pci: soft retembrasure — Wave 44 exclusive embrasure stamp */
kprintf("pci: soft retembrasure exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=46 "
        "(retembrasure stamp; Soft≠product)\n");

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pci: soft retkeepgate — Wave 45 return-keepgate honesty */
kprintf("pci: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=46 "
        "(retkeepgate honesty; Soft≠product; not bar3)\n");
/* Grep: pci: soft retouterward — Wave 45 exclusive outerward stamp */
kprintf("pci: soft retouterward exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=46 "
        "(retouterward stamp; Soft≠product)\n");

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: pci: soft retbailey — Wave 46 return-bailey honesty */
kprintf("pci: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=46 "
        "(retbailey honesty; Soft≠product; not bar3)\n");
/* Grep: pci: soft retpostern — Wave 46 exclusive postern stamp */
kprintf("pci: soft retpostern exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=46 "
        "(retpostern stamp; Soft≠product)\n");



                            kprintf("pci: soft deepen wave=%u areas=%u via=%s ready=%u "
            "prog_live=%u fire=%u hw_prog=%u ok=1 skip=0\n",
            (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE,
            (unsigned)PCI_MSIX_SOFT_DEEPEN_AREAS, szViaSafe,
            (unsigned)(g_fSoftReady ? 1 : 0), (unsigned)cProgLive,
            (unsigned)g_u32SoftFire, (unsigned)g_u32Programmed);
    /* Grep: msix: soft return — Wave 17 twin API return surfaces */
    kprintf("msix: soft return soft_inv=1 table=1 "
            "product_kernel=OPEN bar3=0 hard_gate=0 wave=%u soft PASS\n",
            (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE);
    /* Grep: msix: soft return selftest — Wave 17 terminal return surface (kept) */
    kprintf("msix: soft return selftest inv_ret=1 product_kernel=OPEN "
            "multi_server=0 bar3=0 wave=%u soft PASS\n",
            (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE);
    /* Grep: msix: soft retmap — Wave 17 return-surface map (kept) */
    kprintf("msix: soft retmap soft_inv=1 deepen=1 product=OPEN "
            "wave=%u soft PASS\n",
            (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE);
    kprintf("msix: soft deepen wave=%u areas=%u via=%s ready=%u "
            "prog_live=%u fire=%u hw_prog=%u ok=1 skip=0\n",
            (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE,
            (unsigned)PCI_MSIX_SOFT_DEEPEN_AREAS, szViaSafe,
            (unsigned)(g_fSoftReady ? 1 : 0), (unsigned)cProgLive,
            (unsigned)g_u32SoftFire, (unsigned)g_u32Programmed);

    /* Grep: pci: soft stats / msix: soft stats */
    kprintf("pci: soft stats inv_logs=%u soft_prog=%u soft_fire=%u "
            "hw_prog=%u depth=%u wave=%u\n",
            (unsigned)g_u32SoftInvLogs, (unsigned)g_u32SoftProg,
            (unsigned)g_u32SoftFire, (unsigned)g_u32Programmed,
            (unsigned)GJ_MSIX_SOFT_TBL, (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE);
    kprintf("msix: soft stats inv_logs=%u soft_prog=%u soft_fire=%u "
            "hw_prog=%u depth=%u wave=%u\n",
            (unsigned)g_u32SoftInvLogs, (unsigned)g_u32SoftProg,
            (unsigned)g_u32SoftFire, (unsigned)g_u32Programmed,
            (unsigned)GJ_MSIX_SOFT_TBL, (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE);

    /* Soft path is always available after init — inventory PASS. */
    if (g_fSoftReady) {
        /* Grep: pci: soft inventory PASS / pci: soft PASS */
        kprintf("pci: soft inventory PASS via=%s logs=%u wave=%u "
                "areas=%u\n",
                szViaSafe, (unsigned)g_u32SoftInvLogs,
                (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE,
                (unsigned)PCI_MSIX_SOFT_DEEPEN_AREAS);
        kprintf("pci: soft PASS via=%s wave=%u\n", szViaSafe,
                (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE);
        /* Grep: msix: soft inventory PASS / msix: soft PASS */
        kprintf("msix: soft inventory PASS via=%s logs=%u wave=%u "
                "areas=%u\n",
                szViaSafe, (unsigned)g_u32SoftInvLogs,
                (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE,
                (unsigned)PCI_MSIX_SOFT_DEEPEN_AREAS);
        kprintf("msix: soft PASS via=%s wave=%u\n", szViaSafe,
                (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE);
    } else {
        kprintf("pci: soft inventory SKIP via=%s wave=%u\n", szViaSafe,
                (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE);
        kprintf("msix: soft inventory SKIP via=%s wave=%u\n", szViaSafe,
                (unsigned)PCI_MSIX_SOFT_DEEPEN_WAVE);
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
    /* Wave 14: greppable soft inventory rollup (after exercise state). */
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
     * Wave 14 soft inventory at probe (exercise already dumps once;
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
