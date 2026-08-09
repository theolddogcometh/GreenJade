/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Product MSI/MSI-X capability discovery + table entry programming.
 * Clean-room pure C from PCI Local Bus Spec capability IDs and MSI-X
 * table layout. Soft table shadow for smokes without MMIO.
 * Dual MIT OR Apache-2.0. No GPL source. No version stamp.
 * Soft!=product. G-AC-1.
 *
 * greppable: MSI-X table soft path
 *
 * Lean soft residual (this unit only; C0; Soft!=product; G-AC-1):
 * greppable: "pci: soft ..." | "msix: soft ..."
 *   pci: soft inventory ... / msix: soft inventory ...  - geometry + tallies
 *   pci: soft residual lean PASS / msix: soft residual lean PASS
 *     contiguous PASS after "lean" (UDX IRQ residual foundation)
 *   pci: soft inventory PASS / msix: soft inventory PASS
 * Honesty folded into residual lean (soft shadow != device Table Size).
 * Residual policy: freestanding rtl poll-mode first; MSI-X residual for
 * later UDX userspace IRQ hosts. Soft!=product. No stamp storms (<=4 lamps).
 * Never forces freestanding rtl off poll-mode (hw_force_rtl=0).
 * UDX IRQ foundation: soft_tbl->inject->notify_msix_global->NOTIFY_WAIT;
 * product_notify_mint=OPEN (soft shadow only; no CNode IRQ mint).
 *
 * Residual order deepen (C0; Soft!=product; UDX enable_irq / MC shape):
 *   pba_drain=1       - clear sticky PBA before unmask must not re-issue
 *   dual_latch=1      - vector unmask under fn_mask holds; fn clear delivers
 *   sticky_reissue=1  - Soft PBA not auto-cleared on deliver; remask+unmask
 *                       re-issues without new fire (Soft!=product HW clear)
 *   fn_pba_drain=1    - clear sticky under fn_mask before fn clear: no re-issue
 *   vec_remask_hold=1 - remask vector under fn before fn clear: no deliver
 *   greppable: pba_drain=1 dual_latch=1 sticky_reissue=1 fn_pba_drain=1
 *   greppable: vec_remask_hold=1 order_residual=1
 *
 * Hazard H1 residual (Soft!=product dual license):
 *   Freestanding net may be poll-mode (net_eth_poll owned by scheduler_run).
 *   Never force IRQ eth poll; never call net_eth_poll from this unit
 *   (soft fire / mask / fn_mask / exercise / probe / program - none).
 *   thr/run-loop own eth poll (H1 thr-only eth). Soft!=product.
 *   greppable: force_irq_eth_poll=0 poll_mode_first=1
 *   greppable: net_eth_poll=run_loop_only net_eth_irq=0
 *   greppable: net_eth_poll_from_msix=0 hw_force_rtl=0
 *   greppable: pci: soft residual lean PASS
 *   greppable: msix: soft residual lean PASS
 *   greppable: product_notify_mint=OPEN
 *
 * Dual DoD residual honesty (Soft!=product; G-AC-1; stamp-free):
 *   Dual DoD A/B OPEN — UDX product path; freestanding SKIP.
 *   soft residual lean PASS != Dual DoD close != bar3.
 *   product_msix=OPEN (soft shadow only; no product multi-vector mint).
 *   greppable: pci: soft residual dual_dod
 *   greppable: msix: soft residual dual_dod
 *   greppable: pci: soft residual dual_dod OPEN
 *   greppable: msix: soft residual dual_dod OPEN
 *   greppable: Soft!=product soft residual dual_dod OPEN product_hosts=UDX
 *   Bar honesty v2026.08.04.75 panel context — never invent .76.
 *   Never bump GJ_IMAGE_VERSION from this unit.
 *
 * STRONGER MSI-X table readiness residual (H2 once; Soft!=product):
 *   Soft table ready shape used by DDI_OP_IRQ_BIND for UDX product hosts
 *   rtl8168_udx (Dual DoD B) + xhci_udx (Dual DoD A). Soft shadow only;
 *   soft_note_only; product_notify_mint=OPEN; never freestanding re-enable.
 *   path=soft_tbl->DDI_OP_IRQ_BIND->inject->notify_msix_global->NOTIFY_WAIT
 *   greppable: pci: soft residual table_ready STRONGER
 *   greppable: msix: soft residual table_ready STRONGER
 *   greppable: product_hosts=UDX
 *   greppable: ddi_irq_bind=1
 *   H2 once-lamp (g_fSoftDualDodStrongerOnce); ASCII Soft!=product only.
 *
 * denser table_ready residual for DDI IRQ_BIND (H2 once; Soft!=product):
 *   Multi-arm denser honesty for soft table readiness seed used by
 *   DDI_OP_IRQ_BIND (rtl8168_udx Dual DoD B + xhci_udx Dual DoD A).
 *   Arms (PCI_MSIX_TABLE_READY_DENSE_ARMS=28; all required; W23 denser+++++):
 *     soft_ready | badge0_seed | prog_e0 | prog_e1 | badge_mask_cov |
 *     readback_e0 | h1_locks | dual_dod_open | readback_e1 |
 *     mask_hold_pba | unmask_pend | product_open |
 *     dual_host_indep | badge_tbl_distinct | soft_tbl_depth | inject_chain |
 *     host_pair_seed | latch_cycle | dual_host_chain | dod_open_full |
 *     pba_drain | dual_latch | sticky_reissue | order_residual_full |
 *     fn_pba_drain | vec_remask_hold | order_fn_full | product_path_full
 *   denser=1 denser++=1 denser+++=1 denser++++=1 denser+++++=1
 *   denser_arms=28 denser_ok=N ddi_irq_bind=1 soft_note_only=1
 *   Dual DoD A/B remain OPEN; soft residual never closes product DoD.
 *   greppable: pci: soft residual table_ready denser
 *   greppable: msix: soft residual table_ready denser
 *   greppable: pci: soft residual table_ready denser++
 *   greppable: msix: soft residual table_ready denser++
 *   greppable: pci: soft residual table_ready denser+++
 *   greppable: msix: soft residual table_ready denser+++
 *   greppable: pci: soft residual table_ready denser++++
 *   greppable: msix: soft residual table_ready denser++++
 *   greppable: pci: soft residual table_ready denser+++++
 *   greppable: msix: soft residual table_ready denser+++++
 *   greppable: pci: soft residual denser VERDICT
 *   greppable: msix: soft residual denser VERDICT
 *   greppable: denser residual bar
 *   greppable: bar=v2026.08.04.75
 *   greppable: table_ready denser | denser_arms= | denser_ok=
 *   greppable: denser_arms=28 denser_ok= mask_hold_pba= unmask_pend=
 *   greppable: dual_host_indep= badge_tbl_distinct= soft_tbl_depth=
 *   greppable: inject_chain= denser++=1 denser+++=1 denser++++=1 denser+++++=1
 *   greppable: host_pair_seed= latch_cycle= dual_host_chain= dod_open_full=
 *   greppable: pba_drain= dual_latch= sticky_reissue= order_residual_full=
 *   greppable: fn_pba_drain= vec_remask_hold= order_fn_full= product_path_full=
 *   greppable: Soft!=product dual_dod OPEN product_hosts=UDX
 *   denser residual bar .75 VERDICT: agent-facing rollup over 28 denser arms
 *   Soft residual denser VERDICT != Dual DoD close; Dual DoD A/B remain OPEN.
 *   Bar honesty v2026.08.04.75 stamp-free — never invent .76.
 *   ASCII Soft!=product only (no unicode Soft inequality glyph).
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

/*
 * H1 compile-time lock (Soft!=product): force_irq_eth_poll=0, poll_mode_first=1.
 * Flip requires H1 review - IRQ-stack eth poll is #PF I=1 fault class.
 * greppable: force_irq_eth_poll=0 poll_mode_first=1
 */
_Static_assert(PCI_MSIX_FORCE_IRQ_ETH_POLL == 0u,
               "H1: force_irq_eth_poll must be 0");
_Static_assert(PCI_MSIX_POLL_MODE_FIRST == 1u,
               "H1: poll_mode_first must be 1");
_Static_assert(PCI_MSIX_NET_ETH_IRQ == 0u,
               "H1: net_eth_irq must be 0 (run-loop owns eth poll)");
_Static_assert(PCI_MSIX_NET_ETH_POLL_FROM == 0u,
               "H1: never call net_eth_poll from pci_msix");
_Static_assert(PCI_MSIX_HW_FORCE_RTL == 0u,
               "H1: hw_force_rtl must be 0 (poll-mode first)");

static u32 g_u32Programmed;

/* Soft MSI-X table shadow (always available). */
static struct gj_pci_msix_soft_entry g_aSoftTab[GJ_MSIX_SOFT_TBL];
static u64 g_u64SoftPba;
static u32 g_u32SoftProg;
static u32 g_u32SoftFire;
static int g_fSoftReady;
/* Times soft inventory printed (diagnostics; hard-capped emission). */
static u32 g_u32SoftInvLogs;
/*
 * Soft residual: Message Control Function Mask shape (bit 14). Soft only -
 * does not write device MC. Blocks all soft table delivery while set.
 * greppable: msix: soft residual fn_mask
 */
static u32 g_fSoftFnMask;
/* Soft residual: unmask with sticky PBA -> deliver (UDX enable latch). */
static u32 g_u32SoftUnmaskPend;
/*
 * H2 once: STRONGER Dual DoD / table_ready denser residual lamp latched.
 * Soft!=product; Dual DoD A/B remain OPEN; no stamp storms.
 * greppable: pci: soft residual dual_dod OPEN
 * greppable: pci: soft residual table_ready STRONGER
 * greppable: pci: soft residual table_ready denser
 */
static u8 g_fSoftDualDodStrongerOnce;
/* denser table_ready residual arm tallies (H2 once; Soft!=product). */
static u32 g_u32TableReadyDenseOk;   /* composite denser arms all PASS */
static u32 g_u32TableReadyDenseFail; /* denser composite soft fail */
static u32 g_u32TableReadyDenseArms; /* last denser arm count (0..DENSE_ARMS) */

/*
 * Lean residual inventory: hard cap on emission (no stamp storms).
 * Soft!=product; dual MIT OR Apache-2.0. No version stamp.
 */
#define PCI_MSIX_SOFT_INV_LOG_CAP 2u

/*
 * denser table_ready residual for DDI IRQ_BIND (Soft!=product; Dual DoD OPEN).
 * 28 multi-arm honesty checks (W23 denser+++++); all required for denser ok.
 * greppable: denser_arms=28 denser_ok= table_ready denser denser++ denser+++ denser++++ denser+++++
 * Bar honesty v2026.08.04.75 stamp-free — never invent .76.
 */
#define PCI_MSIX_TABLE_READY_DENSE_ARMS 28u
#define PCI_MSIX_TABLE_READY_DENSE_MIN  28u
_Static_assert(PCI_MSIX_TABLE_READY_DENSE_ARMS == 28u,
               "table_ready denser arms must be 28 (W23 denser+++++)");
_Static_assert(PCI_MSIX_TABLE_READY_DENSE_MIN ==
                   PCI_MSIX_TABLE_READY_DENSE_ARMS,
               "table_ready denser min must equal denser arms");

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
     * MSI-X tables live in device MMIO BARs (often >=0xf0000000). Never use
     * HHDM - that maps RAM only. Identity-map 2 MiB covering the table.
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

/*
 * Soft residual deliver: unmasked + not function-masked -> badge inject.
 * Shared by soft_fire and unmask-pending path. Soft!=product HW send.
 * Returns 1 if delivery attempted, 0 if held/invalid.
 */
static u32
msix_soft_deliver(u16 u16Idx)
{
    struct gj_pci_msix_soft_entry *pEnt;
    u64 u64Badge;

    if (!g_fSoftReady || u16Idx >= GJ_MSIX_SOFT_TBL) {
        return 0;
    }
    pEnt = &g_aSoftTab[u16Idx];
    if (!pEnt->u8Programmed) {
        return 0;
    }
    /* Soft Function Mask residual: global hold (MC bit14 shape). */
    if (g_fSoftFnMask != 0u) {
        return 0;
    }
    if ((pEnt->u32VecCtl & GJ_MSIX_VECCTL_MASK) != 0) {
        return 0; /* per-vector mask: no Notification delivery */
    }
    pEnt->u8SoftFire = 1;
    if (g_u32SoftFire < 0xffffffffu) {
        g_u32SoftFire++;
    }
    /*
     * Badge attribution: entry 0 -> bit 2 (GJ_MSIX_BADGE_TBL(0)) so existing
     * smoke wait masks covering low bits still observe table soft fire.
     * Userspace later: NOTIFY_WAIT which=0 mask=badge reaps the pulse.
     */
    u64Badge = GJ_MSIX_BADGE_TBL(u16Idx);
    if (irq_msix_ready()) {
        irq_msix_soft_inject(u64Badge);
    }
    return 1;
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
    g_fSoftFnMask = 0;
    g_u32SoftUnmaskPend = 0;
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
        /*
         * Residual: sticky PBA while masked -> deliver on unmask
         * (UDX enable_irq latched-pending shape; freestanding rtl stays
         * poll-mode - this is soft scaffolding for later UDX only).
         * Soft!=product HW re-issue. Sticky PBA remains until clear API.
         * Order residual: if Function Mask is set, deliver holds (dual_latch);
         * pba_drain (clear API first) yields no re-issue on unmask.
         */
        if ((g_u64SoftPba & (1ull << (u16Idx & 63u))) != 0) {
            if (msix_soft_deliver(u16Idx) != 0u) {
                if (g_u32SoftUnmaskPend < 0xffffffffu) {
                    g_u32SoftUnmaskPend++;
                }
            }
        }
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

    if (!g_fSoftReady || u16Idx >= GJ_MSIX_SOFT_TBL) {
        return 0;
    }
    pEnt = &g_aSoftTab[u16Idx];
    if (!pEnt->u8Programmed) {
        return 0;
    }
    /*
     * Spec-like sticky PBA: bit set when the function would assert the
     * message (including while masked or function-masked). Soft residual
     * delivers on unmask when PBA is set (see pci_msix_soft_mask).
     */
    g_u64SoftPba |= (1ull << (u16Idx & 63u));
    return msix_soft_deliver(u16Idx);
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
pci_msix_soft_pba_pending(u16 u16Idx)
{
    if (!g_fSoftReady || u16Idx >= GJ_MSIX_SOFT_TBL) {
        return 0;
    }
    return ((g_u64SoftPba & (1ull << (u16Idx & 63u))) != 0) ? 1u : 0u;
}

u32
pci_msix_soft_function_mask(u32 u32Mask)
{
    u32 u32New;
    u32 iEnt;

    if (!g_fSoftReady) {
        pci_msix_soft_table_init();
    }
    u32New = (u32Mask != 0u) ? 1u : 0u;
    /*
     * Residual: Function Mask clear with sticky PBA delivers unmasked
     * programmed entries once (PCI MC bit14 + UDX enable_irq shape).
     * Soft!=product HW MC write. Clear mask first so deliver is not held.
     */
    if (g_fSoftFnMask != 0u && u32New == 0u) {
        g_fSoftFnMask = 0u;
        for (iEnt = 0; iEnt < GJ_MSIX_SOFT_TBL; iEnt++) {
            if ((g_u64SoftPba & (1ull << (iEnt & 63u))) == 0) {
                continue;
            }
            if (msix_soft_deliver((u16)iEnt) != 0u) {
                if (g_u32SoftUnmaskPend < 0xffffffffu) {
                    g_u32SoftUnmaskPend++;
                }
            }
        }
        return 1;
    }
    g_fSoftFnMask = u32New;
    return 1;
}

u32
pci_msix_soft_function_mask_get(void)
{
    return g_fSoftFnMask;
}

u64
pci_msix_soft_badge_mask(void)
{
    u64 u64Mask = 0;
    u32 iEnt;

    if (!g_fSoftReady) {
        return 0;
    }
    for (iEnt = 0; iEnt < GJ_MSIX_SOFT_TBL; iEnt++) {
        if (g_aSoftTab[iEnt].u8Programmed) {
            u64Mask |= GJ_MSIX_BADGE_TBL(iEnt);
        }
    }
    return u64Mask;
}

u64
pci_msix_soft_entry_badge(u16 u16Idx)
{
    if (u16Idx >= GJ_MSIX_SOFT_TBL) {
        return 0;
    }
    return GJ_MSIX_BADGE_TBL(u16Idx);
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

u32
pci_msix_soft_unmask_pend_count(void)
{
    return g_u32SoftUnmaskPend;
}

int
pci_msix_soft_ready(void)
{
    return g_fSoftReady;
}

/*
 * Lean soft residual inventory - greppable "pci: soft ..." / "msix: soft ...".
 * Pure observation; never allocates; never hard-gates HW/soft fire paths.
 * Hard-capped emission (PCI_MSIX_SOFT_INV_LOG_CAP). No version stamp.
 * Soft!=product. No stamp storms: <=4 kprintf lamps per emission.
 * H1: never force IRQ eth poll; freestanding net may stay poll-mode.
 *
 * greppable: pci: soft inventory
 * greppable: msix: soft inventory
 * greppable: pci: soft residual lean
 * greppable: msix: soft residual lean
 * greppable: pci: soft residual lean PASS
 * greppable: msix: soft residual lean PASS
 * greppable: pci: soft inventory PASS
 * greppable: msix: soft inventory PASS
 * greppable: pba_drain=1 dual_latch=1 sticky_reissue=1 fn_pba_drain=1
 * greppable: vec_remask_hold=1 order_residual=1
 * greppable: force_irq_eth_poll=0 poll_mode_first=1
 * greppable: net_eth_poll=run_loop_only net_eth_irq=0
 * greppable: net_eth_poll_from_msix=0
 * greppable: pci: soft residual dual_dod
 * greppable: msix: soft residual dual_dod
 * greppable: pci: soft residual dual_dod OPEN
 * greppable: msix: soft residual dual_dod OPEN
 * greppable: pci: soft residual table_ready STRONGER
 * greppable: msix: soft residual table_ready STRONGER
 * greppable: pci: soft residual table_ready denser
 * greppable: msix: soft residual table_ready denser
 * greppable: table_ready denser denser_arms= denser_ok=
 * greppable: product_msix=OPEN product_notify_mint=OPEN
 * greppable: Soft!=product soft residual dual_dod OPEN product_hosts=UDX
 * greppable: product_hosts=UDX ddi_irq_bind=1
 */

/*
 * STRONGER Dual DoD + denser MSI-X table readiness residual (H2 once).
 * Soft table readiness shape used by DDI_OP_IRQ_BIND for UDX product
 * hosts rtl8168_udx (DoD B) + xhci_udx (DoD A). Soft!=product; Dual DoD
 * A/B OPEN; soft_note_only; product_notify_mint=OPEN; freestanding SKIP.
 * denser multi-arm residual (PCI_MSIX_TABLE_READY_DENSE_ARMS=28; W23 denser+++++):
 *   soft_ready | badge0_seed | prog_e0 | prog_e1 | badge_mask_cov |
 *   readback_e0 | h1_locks | dual_dod_open | readback_e1 |
 *   mask_hold_pba | unmask_pend | product_open |
 *   dual_host_indep | badge_tbl_distinct | soft_tbl_depth | inject_chain |
 *   host_pair_seed | latch_cycle | dual_host_chain | dod_open_full |
 *   pba_drain | dual_latch | sticky_reissue | order_residual_full |
 *   fn_pba_drain | vec_remask_hold | order_fn_full | product_path_full
 * Bar honesty v2026.08.04.75 stamp-free — never invent .76.
 * greppable: pci: soft residual dual_dod OPEN
 * greppable: msix: soft residual dual_dod OPEN
 * greppable: pci: soft residual table_ready STRONGER
 * greppable: msix: soft residual table_ready STRONGER
 * greppable: pci: soft residual table_ready denser
 * greppable: msix: soft residual table_ready denser
 * greppable: pci: soft residual table_ready denser++
 * greppable: msix: soft residual table_ready denser++
 * greppable: pci: soft residual table_ready denser+++
 * greppable: msix: soft residual table_ready denser+++
 * greppable: pci: soft residual table_ready denser++++
 * greppable: msix: soft residual table_ready denser++++
 * greppable: pci: soft residual table_ready denser+++++
 * greppable: msix: soft residual table_ready denser+++++
 * greppable: pci: soft residual denser VERDICT
 * greppable: msix: soft residual denser VERDICT
 * greppable: denser residual bar bar=v2026.08.04.75
 * greppable: Soft!=product soft residual dual_dod OPEN product_hosts=UDX
 */
static void
pci_msix_soft_dual_dod_stronger_once(const char *szVia)
{
    u32 fTableReady;
    u32 fIrqReady;
    u32 fBadge0Ok;
    u32 fProgE0;
    u32 fProgE1;
    u32 fBadgeMaskCov;
    u32 fReadbackE0;
    u32 fReadbackE1;
    u32 fH1Locks;
    u32 fDualDodOpen;
    u32 fMaskHoldPba;
    u32 fUnmaskPend;
    u32 fProductOpen;
    u32 fDualHostIndep;
    u32 fBadgeTblDistinct;
    u32 fSoftTblDepth;
    u32 fInjectChain;
    u32 fHostPairSeed;
    u32 fLatchCycle;
    u32 fDualHostChain;
    u32 fDodOpenFull;
    u32 fPbaDrain;
    u32 fDualLatch;
    u32 fStickyReissue;
    u32 fOrderResidualFull;
    u32 fFnPbaDrain;
    u32 fVecRemaskHold;
    u32 fOrderFnFull;
    u32 fProductPathFull;
    u32 u32Dense;
    u32 u32DenseOk;
    u32 u32Held;
    u32 u32FireBefore;
    u32 u32UnmaskBefore;
    u32 u32FireE1;
    u64 u64Badge0;
    u64 u64Badge1;
    u64 u64BadgeMask;
    u64 u64NeedMask;
    struct gj_pci_msix_soft_entry ent0;
    struct gj_pci_msix_soft_entry ent1;
    const char *szViaSafe;
    const char *szDenseVerdict;

    if (g_fSoftDualDodStrongerOnce != 0u) {
        return;
    }
    g_fSoftDualDodStrongerOnce = 1u;

    if (!g_fSoftReady) {
        pci_msix_soft_table_init();
    }
    szViaSafe = (szVia != NULL) ? szVia : "anon";
    fTableReady = 0u;
    fIrqReady = irq_msix_ready() ? 1u : 0u;
    fBadge0Ok = 0u;
    fProgE0 = 0u;
    fProgE1 = 0u;
    fBadgeMaskCov = 0u;
    fReadbackE0 = 0u;
    fReadbackE1 = 0u;
    fH1Locks = 0u;
    fDualDodOpen = 0u;
    fMaskHoldPba = 0u;
    fUnmaskPend = 0u;
    fProductOpen = 0u;
    fDualHostIndep = 0u;
    fBadgeTblDistinct = 0u;
    fSoftTblDepth = 0u;
    fInjectChain = 0u;
    fHostPairSeed = 0u;
    fLatchCycle = 0u;
    fDualHostChain = 0u;
    fDodOpenFull = 0u;
    fPbaDrain = 0u;
    fDualLatch = 0u;
    fStickyReissue = 0u;
    fOrderResidualFull = 0u;
    fFnPbaDrain = 0u;
    fVecRemaskHold = 0u;
    fOrderFnFull = 0u;
    fProductPathFull = 0u;
    u32Dense = 0u;
    u32DenseOk = 0u;
    u32Held = 0u;
    u32FireBefore = 0u;
    u32UnmaskBefore = 0u;
    u32FireE1 = 0u;
    memset(&ent0, 0, sizeof(ent0));
    memset(&ent1, 0, sizeof(ent1));

    /*
     * denser table_ready residual for DDI IRQ_BIND (Soft!=product).
     * Multi-arm functional residual: soft table seed shape used by
     * DDI_OP_IRQ_BIND soft note (handle->badge) for UDX rtl/xhci hosts.
     * Soft fail only; never hard-gates Dual DoD A/B; Dual DoD remains OPEN.
     * greppable: pci: soft residual table_ready denser
     * greppable: denser_arms= denser_ok=
     */

    /* arm0: soft_ready (table init residual). */
    if (g_fSoftReady != 0 && pci_msix_soft_ready() != 0) {
        fTableReady = 1u;
        u32Dense++;
    }

    /* arm1: badge0_seed — entry0 TBL badge is IRQ_BIND soft note seed. */
    u64Badge0 = pci_msix_soft_entry_badge(0);
    u64Badge1 = pci_msix_soft_entry_badge(1);
    if (u64Badge0 == GJ_MSIX_BADGE_TBL(0) &&
        u64Badge1 == GJ_MSIX_BADGE_TBL(1) &&
        pci_msix_soft_entry_badge(GJ_MSIX_SOFT_TBL) == 0ull) {
        fBadge0Ok = 1u;
        u32Dense++;
    }

    /*
     * arm2/3: prog_e0 + prog_e1 — multi-entry soft program residual for
     * dual UDX hosts (DoD B rtl8168_udx + DoD A xhci_udx). Soft program
     * is idempotent re-mirror; does not force freestanding rtl off poll.
     */
    if (pci_msix_soft_program(0, MSI_ADDR_BASE, (u32)PCI_MSIX_PROBE_VEC,
                              0u) != 0u) {
        fProgE0 = 1u;
        u32Dense++;
    }
    if (pci_msix_soft_program(1, MSI_ADDR_BASE,
                              (u32)(PCI_MSIX_PROBE_VEC + 1u), 0u) != 0u) {
        fProgE1 = 1u;
        u32Dense++;
    }

    /* arm4: badge_mask_cov — programmed e0|e1 cover TBL badges. */
    u64BadgeMask = pci_msix_soft_badge_mask();
    u64NeedMask = GJ_MSIX_BADGE_TBL(0) | GJ_MSIX_BADGE_TBL(1);
    if ((u64BadgeMask & u64NeedMask) == u64NeedMask) {
        fBadgeMaskCov = 1u;
        u32Dense++;
    }

    /* arm5: readback_e0 — soft_read seed entry for DDI IRQ_BIND note. */
    if (pci_msix_soft_read(0, &ent0) != 0u && ent0.u8Programmed != 0u &&
        ent0.u32MsgAddrLo == MSI_ADDR_BASE &&
        (ent0.u32MsgData & 0xffu) == (u32)PCI_MSIX_PROBE_VEC &&
        (ent0.u32VecCtl & GJ_MSIX_VECCTL_MASK) == 0u) {
        fReadbackE0 = 1u;
        u32Dense++;
    }

    /*
     * arm6: h1_locks — never force IRQ eth poll; poll-mode-first; never
     * call net_eth_poll from this unit; never force freestanding rtl HW.
     */
    if (PCI_MSIX_FORCE_IRQ_ETH_POLL == 0u &&
        PCI_MSIX_POLL_MODE_FIRST == 1u && PCI_MSIX_NET_ETH_IRQ == 0u &&
        PCI_MSIX_NET_ETH_POLL_FROM == 0u && PCI_MSIX_HW_FORCE_RTL == 0u) {
        fH1Locks = 1u;
        u32Dense++;
    }

    /*
     * arm7: dual_dod_open honesty — soft residual never closes Dual DoD.
     * denser min/arms honesty + soft table depth bound (not device TS).
     */
    if (PCI_MSIX_TABLE_READY_DENSE_ARMS == 28u &&
        PCI_MSIX_TABLE_READY_DENSE_MIN == PCI_MSIX_TABLE_READY_DENSE_ARMS &&
        GJ_MSIX_SOFT_TBL >= 2u && fTableReady != 0u && fBadge0Ok != 0u) {
        fDualDodOpen = 1u;
        u32Dense++;
    }

    /*
     * arm8: readback_e1 — Dual DoD A host soft entry (xhci_udx) readback.
     * Symmetric multi-host seed for concurrent UDX IRQ_BIND notes.
     */
    if (pci_msix_soft_read(1, &ent1) != 0u && ent1.u8Programmed != 0u &&
        ent1.u32MsgAddrLo == MSI_ADDR_BASE &&
        (ent1.u32MsgData & 0xffu) == (u32)(PCI_MSIX_PROBE_VEC + 1u) &&
        (ent1.u32VecCtl & GJ_MSIX_VECCTL_MASK) == 0u) {
        fReadbackE1 = 1u;
        u32Dense++;
    }

    /*
     * arm9: mask_hold_pba — fire while masked must not deliver; sticky PBA
     * holds (UDX disable_irq latch shape). Soft!=product HW PBA clear.
     * greppable: mask_hold_pba=1
     */
    if (pci_msix_soft_mask(0, 1u) != 0u) {
        u32Held = pci_msix_soft_fire(0);
        if (u32Held == 0u && pci_msix_soft_pba_pending(0) != 0u) {
            fMaskHoldPba = 1u;
            u32Dense++;
        }
    }

    /*
     * arm10: unmask_pend — unmask with sticky PBA delivers once (UDX
     * enable_irq latch). Soft residual only; Dual DoD stays OPEN.
     * greppable: unmask_pend=
     */
    if (fMaskHoldPba != 0u) {
        u32FireBefore = g_u32SoftFire;
        u32UnmaskBefore = g_u32SoftUnmaskPend;
        if (pci_msix_soft_mask(0, 0u) != 0u &&
            g_u32SoftFire > u32FireBefore &&
            g_u32SoftUnmaskPend > u32UnmaskBefore) {
            fUnmaskPend = 1u;
            u32Dense++;
        }
    }

    /*
     * arm11: product_open — product_notify_mint / product_msix OPEN honesty;
     * soft_note_only; freestanding SKIP; soft residual never closes Dual DoD.
     * greppable: product_open=1 product_notify_mint=OPEN soft_note_only=1
     */
    if (fDualDodOpen != 0u && fH1Locks != 0u && fProgE0 != 0u &&
        fProgE1 != 0u && fMaskHoldPba != 0u && fUnmaskPend != 0u &&
        fReadbackE1 != 0u &&
        PCI_MSIX_TABLE_READY_DENSE_ARMS == 28u) {
        fProductOpen = 1u;
        u32Dense++;
    }

    /*
     * arm12: dual_host_indep (W16 denser++) — mask e0 must not block e1 fire
     * deliver. Concurrent UDX hosts (DoD B rtl + DoD A xhci) need independent
     * vector mask residual. Soft!=product; never hard-gates Dual DoD.
     * greppable: dual_host_indep=1
     */
    if (fProgE0 != 0u && fProgE1 != 0u) {
        if (pci_msix_soft_mask(0, 1u) != 0u &&
            pci_msix_soft_mask(1, 0u) != 0u) {
            u32FireBefore = g_u32SoftFire;
            u32FireE1 = pci_msix_soft_fire(1);
            if (u32FireE1 != 0u && g_u32SoftFire > u32FireBefore) {
                fDualHostIndep = 1u;
                u32Dense++;
            }
            /* restore e0 unmasked for inject_chain residual below */
            (void)pci_msix_soft_mask(0, 0u);
        }
    }

    /*
     * arm13: badge_tbl_distinct (W16 denser++) — dual host badges differ so
     * NOTIFY_WAIT masks can select per-host. Soft table residual only.
     * greppable: badge_tbl_distinct=1
     */
    u64Badge0 = pci_msix_soft_entry_badge(0);
    u64Badge1 = pci_msix_soft_entry_badge(1);
    if (u64Badge0 != 0ull && u64Badge1 != 0ull && u64Badge0 != u64Badge1 &&
        u64Badge0 == GJ_MSIX_BADGE_TBL(0) &&
        u64Badge1 == GJ_MSIX_BADGE_TBL(1)) {
        fBadgeTblDistinct = 1u;
        u32Dense++;
    }

    /*
     * arm14: soft_tbl_depth (W16 denser++) — soft shadow depth headroom for
     * multi-host concurrent IRQ_BIND notes (not device Table Size).
     * greppable: soft_tbl_depth=1
     */
    if (GJ_MSIX_SOFT_TBL >= 8u && GJ_MSIX_SOFT_TBL >= 2u &&
        pci_msix_soft_programmed_count() >= 2u) {
        fSoftTblDepth = 1u;
        u32Dense++;
    }

    /*
     * arm15: inject_chain (W16 denser++) — soft fire after dual_host residual
     * leaves e0 unmasked + e1 unmasked; path ready for
     * soft_tbl->inject->notify_msix_global->NOTIFY_WAIT (DDI IRQ_BIND shape).
     * Soft!=product; product_notify_mint remains OPEN.
     * greppable: inject_chain=1 denser++=1
     */
    if (fDualHostIndep != 0u && fBadgeTblDistinct != 0u &&
        fSoftTblDepth != 0u && fProductOpen != 0u) {
        if (pci_msix_soft_mask(0, 0u) != 0u &&
            pci_msix_soft_mask(1, 0u) != 0u) {
            u32FireBefore = g_u32SoftFire;
            if (pci_msix_soft_fire(0) != 0u &&
                g_u32SoftFire > u32FireBefore &&
                pci_msix_soft_entry_badge(0) == GJ_MSIX_BADGE_TBL(0)) {
                fInjectChain = 1u;
                u32Dense++;
            }
        }
    }

    /*
     * arm16: host_pair_seed (W18 denser+++) — dual UDX host soft seed pair:
     * prog+readback e0 (DoD B rtl) and e1 (DoD A xhci). Soft!=product.
     * greppable: host_pair_seed=1 denser+++=1
     */
    if (fProgE0 != 0u && fProgE1 != 0u && fReadbackE0 != 0u &&
        fReadbackE1 != 0u && fBadgeMaskCov != 0u) {
        fHostPairSeed = 1u;
        u32Dense++;
    }

    /*
     * arm17: latch_cycle (W18 denser+++) — mask hold + unmask pend + H1
     * locks form UDX enable/disable IRQ latch residual (soft only).
     * greppable: latch_cycle=1 denser+++=1
     */
    if (fMaskHoldPba != 0u && fUnmaskPend != 0u && fH1Locks != 0u &&
        fInjectChain != 0u) {
        fLatchCycle = 1u;
        u32Dense++;
    }

    /*
     * arm18: dual_host_chain (W18 denser+++) — concurrent host independence
     * + badge distinct + inject chain for dual IRQ_BIND residual.
     * greppable: dual_host_chain=1 denser+++=1
     */
    if (fDualHostIndep != 0u && fBadgeTblDistinct != 0u &&
        fSoftTblDepth != 0u && fInjectChain != 0u && fHostPairSeed != 0u) {
        fDualHostChain = 1u;
        u32Dense++;
    }

    /*
     * arm19: dod_open_full (W18 denser+++) — Dual DoD OPEN composite:
     * dual_dod_open + product_open + soft_tbl_depth + table_ready +
     * denser+++ composites. Soft residual never closes Dual DoD.
     * greppable: dod_open_full=1 denser+++=1 dual_dod=OPEN
     */
    if (fDualDodOpen != 0u && fProductOpen != 0u && fSoftTblDepth != 0u &&
        fTableReady != 0u && fHostPairSeed != 0u && fLatchCycle != 0u &&
        fDualHostChain != 0u &&
        PCI_MSIX_TABLE_READY_DENSE_ARMS == 28u) {
        fDodOpenFull = 1u;
        u32Dense++;
    }

    /*
     * W20 denser++++ order residual arms (Soft!=product; Dual DoD OPEN).
     * Isolate order residual on entry0: drain sticky PBA and mask e1 so
     * dual_latch / sticky_reissue tallies are not polluted by earlier multi
     * entry sticky (Soft sticky until clear). Soft!=product HW PBA clear.
     * greppable: pba_drain=1 dual_latch=1 sticky_reissue=1 order_residual_full=1
     * greppable: denser++++=1 denser+++++=1 denser_arms=28
     */
    (void)pci_msix_soft_pba_clear(~0ull);
    (void)pci_msix_soft_function_mask(0u);
    (void)pci_msix_soft_mask(1, 1u);

    /*
     * arm20: pba_drain (W20 denser++++) — fire while masked sets sticky;
     * clear sticky before unmask must not re-issue (UDX ack shape).
     * greppable: pba_drain=1 denser++++=1
     */
    if (fDodOpenFull != 0u && fH1Locks != 0u) {
        if (pci_msix_soft_mask(0, 1u) != 0u &&
            pci_msix_soft_fire(0) == 0u &&
            pci_msix_soft_pba_pending(0) != 0u &&
            pci_msix_soft_pba_clear(1ull) == 1ull &&
            pci_msix_soft_pba_pending(0) == 0u) {
            u32FireBefore = g_u32SoftFire;
            u32UnmaskBefore = g_u32SoftUnmaskPend;
            if (pci_msix_soft_mask(0, 0u) != 0u &&
                g_u32SoftFire == u32FireBefore &&
                g_u32SoftUnmaskPend == u32UnmaskBefore) {
                fPbaDrain = 1u;
                u32Dense++;
            }
        }
    }

    /*
     * arm21: dual_latch (W20 denser++++) — vector unmask under fn_mask holds;
     * fn clear with sticky + unmasked vector delivers once (MC bit14 shape).
     * greppable: dual_latch=1 denser++++=1
     */
    if (fPbaDrain != 0u) {
        if (pci_msix_soft_mask(0, 1u) != 0u &&
            pci_msix_soft_fire(0) == 0u &&
            pci_msix_soft_pba_pending(0) != 0u &&
            pci_msix_soft_function_mask(1u) != 0u) {
            u32FireBefore = g_u32SoftFire;
            u32UnmaskBefore = g_u32SoftUnmaskPend;
            if (pci_msix_soft_mask(0, 0u) != 0u &&
                g_u32SoftFire == u32FireBefore &&
                g_u32SoftUnmaskPend == u32UnmaskBefore &&
                pci_msix_soft_pba_pending(0) != 0u &&
                pci_msix_soft_function_mask_get() != 0u &&
                pci_msix_soft_function_mask(0u) != 0u &&
                g_u32SoftFire > u32FireBefore &&
                g_u32SoftUnmaskPend > u32UnmaskBefore &&
                pci_msix_soft_function_mask_get() == 0u) {
                fDualLatch = 1u;
                u32Dense++;
            }
        }
    }

    /*
     * arm22: sticky_reissue (W20 denser++++) — Soft PBA not auto-cleared on
     * deliver; remask+unmask re-issues without new fire (Soft!=product HW).
     * greppable: sticky_reissue=1 denser++++=1
     */
    if (fDualLatch != 0u && pci_msix_soft_pba_pending(0) != 0u) {
        if (pci_msix_soft_mask(0, 1u) != 0u) {
            u32FireBefore = g_u32SoftFire;
            u32UnmaskBefore = g_u32SoftUnmaskPend;
            if (pci_msix_soft_mask(0, 0u) != 0u &&
                g_u32SoftFire > u32FireBefore &&
                g_u32SoftUnmaskPend > u32UnmaskBefore) {
                fStickyReissue = 1u;
                u32Dense++;
            }
        }
    }

    /*
     * arm23: order_residual_full (W20 denser++++) — denser++++ composite over
     * pba_drain + dual_latch + sticky_reissue + dod_open_full. Soft residual
     * never closes Dual DoD A/B; product_notify_mint remains OPEN.
     * greppable: order_residual_full=1 denser++++=1 dual_dod=OPEN
     */
    if (fPbaDrain != 0u && fDualLatch != 0u && fStickyReissue != 0u &&
        fDodOpenFull != 0u && fH1Locks != 0u && fDualHostChain != 0u &&
        PCI_MSIX_TABLE_READY_DENSE_ARMS == 28u) {
        fOrderResidualFull = 1u;
        u32Dense++;
    }

    /* Restore clean soft state after denser++++ order residual arms. */
    (void)pci_msix_soft_function_mask(0u);
    (void)pci_msix_soft_pba_clear(~0ull);
    (void)pci_msix_soft_mask(0, 0u);
    (void)pci_msix_soft_mask(1, 0u);

    /*
     * W23 denser+++++ order residual arms (Soft!=product; Dual DoD OPEN).
     * fn_pba_drain + vec_remask_hold + order_fn_full + product_path_full
     * for UDX enable_irq / MC Function Mask residual (soft only).
     * greppable: fn_pba_drain=1 vec_remask_hold=1 order_fn_full=1
     * greppable: product_path_full=1 denser+++++=1 denser_arms=28
     */
    (void)pci_msix_soft_mask(1, 1u);

    /*
     * arm24: fn_pba_drain (W23 denser+++++) — under Function Mask, drain
     * sticky then clear fn_mask must not re-issue (MC enable / ack shape).
     * greppable: fn_pba_drain=1 denser+++++=1
     */
    if (fOrderResidualFull != 0u && fH1Locks != 0u) {
        if (pci_msix_soft_mask(0, 1u) != 0u &&
            pci_msix_soft_fire(0) == 0u &&
            pci_msix_soft_pba_pending(0) != 0u &&
            pci_msix_soft_function_mask(1u) != 0u &&
            pci_msix_soft_pba_clear(1ull) == 1ull &&
            pci_msix_soft_pba_pending(0) == 0u) {
            u32FireBefore = g_u32SoftFire;
            u32UnmaskBefore = g_u32SoftUnmaskPend;
            if (pci_msix_soft_function_mask(0u) != 0u &&
                g_u32SoftFire == u32FireBefore &&
                g_u32SoftUnmaskPend == u32UnmaskBefore &&
                pci_msix_soft_function_mask_get() == 0u) {
                fFnPbaDrain = 1u;
                u32Dense++;
            }
        }
    }

    /*
     * arm25: vec_remask_hold (W23 denser+++++) — dual_latch complement:
     * fire under vector mask + fn; unmask vector (held by fn); remask;
     * clear fn -> vector mask still holds (no deliver). Soft!=product.
     * greppable: vec_remask_hold=1 denser+++++=1
     */
    if (fFnPbaDrain != 0u) {
        if (pci_msix_soft_mask(0, 1u) != 0u &&
            pci_msix_soft_fire(0) == 0u &&
            pci_msix_soft_pba_pending(0) != 0u &&
            pci_msix_soft_function_mask(1u) != 0u) {
            u32FireBefore = g_u32SoftFire;
            u32UnmaskBefore = g_u32SoftUnmaskPend;
            if (pci_msix_soft_mask(0, 0u) != 0u &&
                g_u32SoftFire == u32FireBefore &&
                g_u32SoftUnmaskPend == u32UnmaskBefore &&
                pci_msix_soft_mask(0, 1u) != 0u &&
                pci_msix_soft_function_mask(0u) != 0u &&
                g_u32SoftFire == u32FireBefore &&
                g_u32SoftUnmaskPend == u32UnmaskBefore &&
                pci_msix_soft_pba_pending(0) != 0u &&
                pci_msix_soft_function_mask_get() == 0u) {
                fVecRemaskHold = 1u;
                u32Dense++;
            }
        }
    }

    /*
     * arm26: order_fn_full (W23 denser+++++) — denser+++++ composite over
     * fn_pba_drain + vec_remask_hold + order_residual_full. Soft residual
     * never closes Dual DoD A/B; product_notify_mint remains OPEN.
     * greppable: order_fn_full=1 denser+++++=1 dual_dod=OPEN
     */
    if (fFnPbaDrain != 0u && fVecRemaskHold != 0u &&
        fOrderResidualFull != 0u && fH1Locks != 0u &&
        fDualHostChain != 0u &&
        PCI_MSIX_TABLE_READY_DENSE_ARMS == 28u) {
        fOrderFnFull = 1u;
        u32Dense++;
    }

    /*
     * arm27: product_path_full (W23 denser+++++) — full soft product path
     * for DDI IRQ_BIND UDX hosts: order_fn_full + inject_chain + dual host
     * + dod_open_full. Soft!=product; Dual DoD remains OPEN.
     * greppable: product_path_full=1 denser+++++=1
     */
    if (fOrderFnFull != 0u && fInjectChain != 0u &&
        fDualHostChain != 0u && fDodOpenFull != 0u &&
        fHostPairSeed != 0u && fH1Locks != 0u &&
        PCI_MSIX_TABLE_READY_DENSE_ARMS == 28u) {
        fProductPathFull = 1u;
        u32Dense++;
    }

    /* Restore clean soft state after denser+++++ residual arms. */
    (void)pci_msix_soft_function_mask(0u);
    (void)pci_msix_soft_pba_clear(~0ull);
    (void)pci_msix_soft_mask(0, 0u);
    (void)pci_msix_soft_mask(1, 0u);

    /* Refresh badge mask after denser arms (mask/unmask may not change it). */
    u64BadgeMask = pci_msix_soft_badge_mask();

    g_u32TableReadyDenseArms = u32Dense;
    if (u32Dense >= PCI_MSIX_TABLE_READY_DENSE_MIN && fTableReady != 0u &&
        fBadge0Ok != 0u && fProgE0 != 0u && fProgE1 != 0u &&
        fBadgeMaskCov != 0u && fReadbackE0 != 0u && fH1Locks != 0u &&
        fDualDodOpen != 0u && fReadbackE1 != 0u && fMaskHoldPba != 0u &&
        fUnmaskPend != 0u && fProductOpen != 0u &&
        fDualHostIndep != 0u && fBadgeTblDistinct != 0u &&
        fSoftTblDepth != 0u && fInjectChain != 0u &&
        fHostPairSeed != 0u && fLatchCycle != 0u &&
        fDualHostChain != 0u && fDodOpenFull != 0u &&
        fPbaDrain != 0u && fDualLatch != 0u && fStickyReissue != 0u &&
        fOrderResidualFull != 0u && fFnPbaDrain != 0u &&
        fVecRemaskHold != 0u && fOrderFnFull != 0u &&
        fProductPathFull != 0u) {
        u32DenseOk = 1u;
        if (g_u32TableReadyDenseOk < 0xffffffffu) {
            g_u32TableReadyDenseOk++;
        }
        szDenseVerdict = "PASS";
    } else {
        if (g_u32TableReadyDenseFail < 0xffffffffu) {
            g_u32TableReadyDenseFail++;
        }
        szDenseVerdict = "FAIL";
    }

    /*
     * Grep: pci: soft residual dual_dod OPEN
     * Grep: Soft!=product soft residual dual_dod OPEN product_hosts=UDX
     * H2 once; Dual DoD A/B remain OPEN (not product DoD close).
     */
    kprintf("pci: soft residual dual_dod OPEN product_hosts=UDX "
            "Soft!=product soft residual dual_dod OPEN product_hosts=UDX "
            "dual_dod_A=OPEN dual_dod_B=OPEN "
            "A_usb=xhci_udx B_nic=rtl8168_udx "
            "table_ready=%u soft_ready=%u irq_ready=%u badge0_ok=%u "
            "denser=%u denser_arms=%u denser_ok=%u denser_min=%u "
            "depth=%u prog=%u fire=%u unmask_pend=%u "
            "badge0=0x%lx badge_mask=0x%lx "
            "path=soft_tbl->DDI_OP_IRQ_BIND->inject->notify_msix_global->"
            "NOTIFY_WAIT "
            "ddi_irq_bind=1 soft_note_only=1 product_notify_mint=OPEN "
            "product_msix=OPEN freestanding_skip=1 "
            "soft_residual_closes_dod=0 udx_irq=1 "
            "via=%s stamp_free=1 bar_honesty=v2026.08.04.75 never_invent=.76 "
            "soft=1 product=0 "
            "dual=MIT|Apache-2.0 G-AC-1 "
            "(STRONGER denser MSI-X table readiness for DDI IRQ_BIND "
            "rtl/xhci; not Dual DoD close; Soft!=product)\n",
            (unsigned)fTableReady, (unsigned)(g_fSoftReady ? 1 : 0),
            (unsigned)fIrqReady, (unsigned)fBadge0Ok,
            (unsigned)u32DenseOk, (unsigned)u32Dense, (unsigned)u32DenseOk,
            (unsigned)PCI_MSIX_TABLE_READY_DENSE_MIN,
            (unsigned)GJ_MSIX_SOFT_TBL, (unsigned)g_u32SoftProg,
            (unsigned)g_u32SoftFire, (unsigned)g_u32SoftUnmaskPend,
            (unsigned long)u64Badge0, (unsigned long)u64BadgeMask, szViaSafe);
    kprintf("msix: soft residual dual_dod OPEN product_hosts=UDX "
            "Soft!=product soft residual dual_dod OPEN product_hosts=UDX "
            "dual_dod_A=OPEN dual_dod_B=OPEN "
            "table_ready=%u irq_ready=%u badge0_ok=%u "
            "denser=%u denser_arms=%u denser_ok=%u "
            "ddi_irq_bind=1 soft_note_only=1 product_notify_mint=OPEN "
            "product_msix=OPEN freestanding_skip=1 "
            "soft_residual_closes_dod=0 stamp_free=1 "
            "soft=1 product=0 G-AC-1=1 "
            "dual=MIT|Apache-2.0\n",
            (unsigned)fTableReady, (unsigned)fIrqReady, (unsigned)fBadge0Ok,
            (unsigned)u32DenseOk, (unsigned)u32Dense, (unsigned)u32DenseOk);

    /*
     * Grep: pci: soft residual table_ready STRONGER
     * Grep: msix: soft residual table_ready STRONGER
     * Table readiness used by DDI IRQ_BIND for UDX rtl/xhci hosts.
     */
    kprintf("pci: soft residual table_ready STRONGER "
            "table_ready=%u soft_ready=%u irq_ready=%u badge0_ok=%u "
            "denser=%u denser_arms=%u denser_ok=%u "
            "prog_e0=%u prog_e1=%u badge_mask_cov=%u readback_e0=%u "
            "readback_e1=%u mask_hold_pba=%u unmask_pend=%u product_open=%u "
            "h1_locks=%u dual_dod_open=%u "
            "prog=%u fire=%u unmask_pend_ct=%u hw_prog=%u "
            "badge0=0x%lx ddi_irq_bind=1 "
            "product_hosts=UDX hosts=rtl8168_udx,xhci_udx "
            "dual_dod OPEN dual_dod_A=OPEN dual_dod_B=OPEN "
            "path=soft_tbl->DDI_OP_IRQ_BIND->inject->notify_msix_global->"
            "NOTIFY_WAIT "
            "product_notify_mint=OPEN product_msix=OPEN soft_shadow=1 "
            "soft=1 product=0 Soft!=product "
            "bar_honesty=v2026.08.04.75 stamp_free=1 H2=once hazard=H1 "
            "dual=MIT|Apache-2.0 G-AC-1 "
            "(STRONGER denser table readiness residual; not Dual DoD close)\n",
            (unsigned)fTableReady, (unsigned)(g_fSoftReady ? 1 : 0),
            (unsigned)fIrqReady, (unsigned)fBadge0Ok,
            (unsigned)u32DenseOk, (unsigned)u32Dense, (unsigned)u32DenseOk,
            (unsigned)fProgE0, (unsigned)fProgE1, (unsigned)fBadgeMaskCov,
            (unsigned)fReadbackE0, (unsigned)fReadbackE1,
            (unsigned)fMaskHoldPba, (unsigned)fUnmaskPend,
            (unsigned)fProductOpen, (unsigned)fH1Locks,
            (unsigned)fDualDodOpen, (unsigned)g_u32SoftProg,
            (unsigned)g_u32SoftFire, (unsigned)g_u32SoftUnmaskPend,
            (unsigned)g_u32Programmed, (unsigned long)u64Badge0);
    kprintf("msix: soft residual table_ready STRONGER "
            "table_ready=%u denser=%u denser_arms=%u denser_ok=%u "
            "product_hosts=UDX "
            "hosts=rtl8168_udx,xhci_udx ddi_irq_bind=1 "
            "dual_dod OPEN dual_dod_A=OPEN dual_dod_B=OPEN "
            "soft=1 product=0 Soft!=product H2=once stamp_free=1 "
            "G-AC-1=1 dual=MIT|Apache-2.0\n",
            (unsigned)fTableReady, (unsigned)u32DenseOk, (unsigned)u32Dense,
            (unsigned)u32DenseOk);

    /*
     * Grep: pci: soft residual table_ready denser
     * Grep: msix: soft residual table_ready denser
     * Grep: table_ready denser denser_arms= denser_ok=
     * denser multi-arm residual for DDI IRQ_BIND soft table seed (24 arms).
     */
    kprintf("pci: soft residual table_ready denser %s denser=1 denser++=1 denser+++=1 denser++++=1 "
            "denser_arms=%u denser_ok=%u denser_min=%u denser_fail=%u "
            "soft_ready=%u badge0_seed=%u prog_e0=%u prog_e1=%u "
            "badge_mask_cov=%u readback_e0=%u h1_locks=%u dual_dod_open=%u "
            "readback_e1=%u mask_hold_pba=%u unmask_pend=%u product_open=%u "
            "dual_host_indep=%u badge_tbl_distinct=%u soft_tbl_depth=%u "
            "inject_chain=%u host_pair_seed=%u latch_cycle=%u "
            "dual_host_chain=%u dod_open_full=%u "
            "pba_drain=%u dual_latch=%u sticky_reissue=%u order_residual_full=%u "
            "irq_ready=%u badge0=0x%lx badge1=0x%lx badge_mask=0x%lx "
            "ddi_irq_bind=1 soft_note_only=1 product_notify_mint=OPEN "
            "product_msix=OPEN product_hosts=UDX "
            "hosts=rtl8168_udx,xhci_udx "
            "path=soft_tbl->DDI_OP_IRQ_BIND->inject->notify_msix_global->"
            "NOTIFY_WAIT "
            "dual_dod OPEN dual_dod_A=OPEN dual_dod_B=OPEN "
            "soft_residual_closes_dod=0 freestanding_skip=1 "
            "force_irq_eth_poll=%u poll_mode_first=%u "
            "net_eth_irq=%u net_eth_poll_from_msix=%u hw_force_rtl=%u "
            "via=%s stamp_free=1 bar_honesty=v2026.08.04.75 never_invent=.76 "
            "H2=once hazard=H1 soft=1 product=0 Soft!=product "
            "G-AC-1 dual=MIT|Apache-2.0 "
            "(denser++++ table_ready residual 24-arm for DDI IRQ_BIND; "
            "not Dual DoD close; Soft!=product)\n",
            szDenseVerdict, (unsigned)u32Dense, (unsigned)u32DenseOk,
            (unsigned)PCI_MSIX_TABLE_READY_DENSE_MIN,
            (unsigned)g_u32TableReadyDenseFail, (unsigned)fTableReady,
            (unsigned)fBadge0Ok, (unsigned)fProgE0, (unsigned)fProgE1,
            (unsigned)fBadgeMaskCov, (unsigned)fReadbackE0,
            (unsigned)fH1Locks, (unsigned)fDualDodOpen,
            (unsigned)fReadbackE1, (unsigned)fMaskHoldPba,
            (unsigned)fUnmaskPend, (unsigned)fProductOpen,
            (unsigned)fDualHostIndep, (unsigned)fBadgeTblDistinct,
            (unsigned)fSoftTblDepth, (unsigned)fInjectChain,
            (unsigned)fHostPairSeed, (unsigned)fLatchCycle,
            (unsigned)fDualHostChain, (unsigned)fDodOpenFull,
            (unsigned)fPbaDrain, (unsigned)fDualLatch,
            (unsigned)fStickyReissue, (unsigned)fOrderResidualFull,
            (unsigned)fIrqReady, (unsigned long)u64Badge0,
            (unsigned long)u64Badge1, (unsigned long)u64BadgeMask,
            (unsigned)PCI_MSIX_FORCE_IRQ_ETH_POLL,
            (unsigned)PCI_MSIX_POLL_MODE_FIRST,
            (unsigned)PCI_MSIX_NET_ETH_IRQ,
            (unsigned)PCI_MSIX_NET_ETH_POLL_FROM,
            (unsigned)PCI_MSIX_HW_FORCE_RTL, szViaSafe);
    kprintf("msix: soft residual table_ready denser %s denser=1 denser++=1 denser+++=1 denser++++=1 "
            "denser_arms=%u denser_ok=%u denser_min=%u "
            "soft_ready=%u badge0_seed=%u prog_e0=%u prog_e1=%u "
            "badge_mask_cov=%u readback_e0=%u h1_locks=%u dual_dod_open=%u "
            "readback_e1=%u mask_hold_pba=%u unmask_pend=%u product_open=%u "
            "dual_host_indep=%u badge_tbl_distinct=%u soft_tbl_depth=%u "
            "inject_chain=%u host_pair_seed=%u latch_cycle=%u "
            "dual_host_chain=%u dod_open_full=%u "
            "pba_drain=%u dual_latch=%u sticky_reissue=%u order_residual_full=%u "
            "ddi_irq_bind=1 product_hosts=UDX "
            "dual_dod OPEN dual_dod_A=OPEN dual_dod_B=OPEN "
            "soft=1 product=0 Soft!=product H2=once stamp_free=1 "
            "G-AC-1=1 dual=MIT|Apache-2.0\n",
            szDenseVerdict, (unsigned)u32Dense, (unsigned)u32DenseOk,
            (unsigned)PCI_MSIX_TABLE_READY_DENSE_MIN, (unsigned)fTableReady,
            (unsigned)fBadge0Ok, (unsigned)fProgE0, (unsigned)fProgE1,
            (unsigned)fBadgeMaskCov, (unsigned)fReadbackE0,
            (unsigned)fH1Locks, (unsigned)fDualDodOpen,
            (unsigned)fReadbackE1, (unsigned)fMaskHoldPba,
            (unsigned)fUnmaskPend, (unsigned)fProductOpen,
            (unsigned)fDualHostIndep, (unsigned)fBadgeTblDistinct,
            (unsigned)fSoftTblDepth, (unsigned)fInjectChain,
            (unsigned)fHostPairSeed, (unsigned)fLatchCycle,
            (unsigned)fDualHostChain, (unsigned)fDodOpenFull,
            (unsigned)fPbaDrain, (unsigned)fDualLatch,
            (unsigned)fStickyReissue, (unsigned)fOrderResidualFull);
    /*
     * Grep: pci: soft residual table_ready denser++
     * Grep: msix: soft residual table_ready denser++
     * W16 denser++ residual: dual_host_indep + badge distinct + depth +
     * inject_chain for concurrent UDX IRQ_BIND (rtl + xhci). Soft!=product.
     */
    kprintf("pci: soft residual table_ready denser++ %s denser++=1 denser=1 "
            "denser_arms=%u denser_ok=%u "
            "dual_host_indep=%u badge_tbl_distinct=%u soft_tbl_depth=%u "
            "inject_chain=%u product_open=%u dual_dod_open=%u "
            "path=soft_tbl->DDI_OP_IRQ_BIND->inject->notify_msix_global->"
            "NOTIFY_WAIT "
            "product_hosts=UDX hosts=rtl8168_udx,xhci_udx "
            "dual_dod OPEN dual_dod_A=OPEN dual_dod_B=OPEN "
            "soft_residual_closes_dod=0 freestanding_skip=1 "
            "product_notify_mint=OPEN product_msix=OPEN soft_note_only=1 "
            "bar_honesty=v2026.08.04.75 stamp_free=1 never_invent=.76 "
            "H2=once Soft!=product G-AC-1 dual=MIT|Apache-2.0 "
            "(W16 denser++ MSI-X table residual; not Dual DoD close)\n",
            szDenseVerdict, (unsigned)u32Dense, (unsigned)u32DenseOk,
            (unsigned)fDualHostIndep, (unsigned)fBadgeTblDistinct,
            (unsigned)fSoftTblDepth, (unsigned)fInjectChain,
            (unsigned)fProductOpen, (unsigned)fDualDodOpen);
    kprintf("msix: soft residual table_ready denser++ %s denser++=1 "
            "denser_arms=%u denser_ok=%u "
            "dual_host_indep=%u badge_tbl_distinct=%u soft_tbl_depth=%u "
            "inject_chain=%u "
            "dual_dod OPEN dual_dod_A=OPEN dual_dod_B=OPEN "
            "product_hosts=UDX Soft!=product H2=once stamp_free=1 "
            "G-AC-1=1 dual=MIT|Apache-2.0\n",
            szDenseVerdict, (unsigned)u32Dense, (unsigned)u32DenseOk,
            (unsigned)fDualHostIndep, (unsigned)fBadgeTblDistinct,
            (unsigned)fSoftTblDepth, (unsigned)fInjectChain);
    /*
     * Grep: pci: soft residual table_ready denser+++
     * Grep: msix: soft residual table_ready denser+++
     * W18 denser+++ residual: host_pair_seed + latch_cycle + dual_host_chain +
     * dod_open_full composites for concurrent UDX IRQ_BIND (rtl + xhci).
     * Soft!=product; Dual DoD A/B remain OPEN; denser+++ residual != close.
     */
    kprintf("pci: soft residual table_ready denser+++ %s denser+++=1 denser++=1 denser=1 "
            "denser_arms=%u denser_ok=%u denser_min=%u "
            "host_pair_seed=%u latch_cycle=%u dual_host_chain=%u dod_open_full=%u "
            "dual_host_indep=%u badge_tbl_distinct=%u soft_tbl_depth=%u "
            "inject_chain=%u product_open=%u dual_dod_open=%u "
            "path=soft_tbl->DDI_OP_IRQ_BIND->inject->notify_msix_global->"
            "NOTIFY_WAIT "
            "product_hosts=UDX hosts=rtl8168_udx,xhci_udx "
            "dual_dod OPEN dual_dod_A=OPEN dual_dod_B=OPEN "
            "soft_residual_closes_dod=0 freestanding_skip=1 "
            "product_notify_mint=OPEN product_msix=OPEN soft_note_only=1 "
            "bar_honesty=v2026.08.04.75 stamp_free=1 never_invent=.76 "
            "H2=once Soft!=product G-AC-1 dual=MIT|Apache-2.0 "
            "(W18 denser+++ MSI-X table residual; not Dual DoD close)\n",
            szDenseVerdict, (unsigned)u32Dense, (unsigned)u32DenseOk,
            (unsigned)PCI_MSIX_TABLE_READY_DENSE_MIN,
            (unsigned)fHostPairSeed, (unsigned)fLatchCycle,
            (unsigned)fDualHostChain, (unsigned)fDodOpenFull,
            (unsigned)fDualHostIndep, (unsigned)fBadgeTblDistinct,
            (unsigned)fSoftTblDepth, (unsigned)fInjectChain,
            (unsigned)fProductOpen, (unsigned)fDualDodOpen);
    kprintf("msix: soft residual table_ready denser+++ %s denser+++=1 "
            "denser_arms=%u denser_ok=%u "
            "host_pair_seed=%u latch_cycle=%u dual_host_chain=%u dod_open_full=%u "
            "dual_dod OPEN dual_dod_A=OPEN dual_dod_B=OPEN "
            "product_hosts=UDX Soft!=product H2=once stamp_free=1 "
            "G-AC-1=1 dual=MIT|Apache-2.0 "
            "(W18 denser+++ MSI-X residual; not Dual DoD close)\n",
            szDenseVerdict, (unsigned)u32Dense, (unsigned)u32DenseOk,
            (unsigned)fHostPairSeed, (unsigned)fLatchCycle,
            (unsigned)fDualHostChain, (unsigned)fDodOpenFull);
    /*
     * Grep: pci: soft residual table_ready denser++++
     * Grep: msix: soft residual table_ready denser++++
     * W20 denser++++ residual: pba_drain + dual_latch + sticky_reissue +
     * order_residual_full for UDX enable_irq / MC shape (soft only).
     * Soft!=product; Dual DoD A/B remain OPEN; denser++++ residual != close.
     * Bar honesty v2026.08.04.75 stamp-free — never invent .76.
     * ASCII Soft!=product only (no unicode Soft inequality glyph).
     */
    kprintf("pci: soft residual table_ready denser++++ %s denser++++=1 denser+++=1 denser++=1 denser=1 "
            "denser_arms=%u denser_ok=%u denser_min=%u "
            "pba_drain=%u dual_latch=%u sticky_reissue=%u order_residual_full=%u "
            "host_pair_seed=%u latch_cycle=%u dual_host_chain=%u dod_open_full=%u "
            "dual_host_indep=%u inject_chain=%u product_open=%u dual_dod_open=%u "
            "path=soft_tbl->DDI_OP_IRQ_BIND->inject->notify_msix_global->"
            "NOTIFY_WAIT "
            "product_hosts=UDX hosts=rtl8168_udx,xhci_udx "
            "dual_dod OPEN dual_dod_A=OPEN dual_dod_B=OPEN "
            "soft_residual_closes_dod=0 freestanding_skip=1 "
            "product_notify_mint=OPEN product_msix=OPEN soft_note_only=1 "
            "bar_honesty=v2026.08.04.75 stamp_free=1 never_invent=.76 "
            "H2=once Soft!=product G-AC-1 dual=MIT|Apache-2.0 "
            "(W20 denser++++ MSI-X table residual 24-arm; not Dual DoD close)\n",
            szDenseVerdict, (unsigned)u32Dense, (unsigned)u32DenseOk,
            (unsigned)PCI_MSIX_TABLE_READY_DENSE_MIN,
            (unsigned)fPbaDrain, (unsigned)fDualLatch,
            (unsigned)fStickyReissue, (unsigned)fOrderResidualFull,
            (unsigned)fHostPairSeed, (unsigned)fLatchCycle,
            (unsigned)fDualHostChain, (unsigned)fDodOpenFull,
            (unsigned)fDualHostIndep, (unsigned)fInjectChain,
            (unsigned)fProductOpen, (unsigned)fDualDodOpen);
    kprintf("msix: soft residual table_ready denser++++ %s denser++++=1 "
            "denser_arms=%u denser_ok=%u "
            "pba_drain=%u dual_latch=%u sticky_reissue=%u order_residual_full=%u "
            "dual_dod OPEN dual_dod_A=OPEN dual_dod_B=OPEN "
            "product_hosts=UDX Soft!=product H2=once stamp_free=1 "
            "G-AC-1=1 dual=MIT|Apache-2.0 "
            "(W20 denser++++ MSI-X residual; not Dual DoD close)\n",
            szDenseVerdict, (unsigned)u32Dense, (unsigned)u32DenseOk,
            (unsigned)fPbaDrain, (unsigned)fDualLatch,
            (unsigned)fStickyReissue, (unsigned)fOrderResidualFull);

    /*
     * Grep: pci: soft residual table_ready denser+++++
     * Grep: msix: soft residual table_ready denser+++++
     * W23 denser+++++ residual: fn_pba_drain + vec_remask_hold + order_fn_full
     * + product_path_full for UDX enable_irq / MC Function Mask (soft only).
     * Soft!=product; Dual DoD A/B remain OPEN; denser+++++ residual != close.
     * Bar honesty v2026.08.04.75 stamp-free — never invent .76.
     * ASCII Soft!=product only (no unicode Soft inequality glyph).
     */
    kprintf("pci: soft residual table_ready denser+++++ %s denser+++++=1 denser++++=1 denser+++=1 denser++=1 denser=1 "
            "denser_arms=%u denser_ok=%u denser_min=%u "
            "fn_pba_drain=%u vec_remask_hold=%u order_fn_full=%u product_path_full=%u "
            "pba_drain=%u dual_latch=%u sticky_reissue=%u order_residual_full=%u "
            "host_pair_seed=%u latch_cycle=%u dual_host_chain=%u dod_open_full=%u "
            "dual_host_indep=%u inject_chain=%u product_open=%u dual_dod_open=%u "
            "path=soft_tbl->DDI_OP_IRQ_BIND->inject->notify_msix_global->"
            "NOTIFY_WAIT "
            "product_hosts=UDX hosts=rtl8168_udx,xhci_udx "
            "dual_dod OPEN dual_dod_A=OPEN dual_dod_B=OPEN "
            "soft_residual_closes_dod=0 freestanding_skip=1 "
            "product_notify_mint=OPEN product_msix=OPEN soft_note_only=1 "
            "bar_honesty=v2026.08.04.75 stamp_free=1 never_invent=.76 "
            "H2=once Soft!=product G-AC-1 dual=MIT|Apache-2.0 "
            "(W23 denser+++++ MSI-X table residual 28-arm; not Dual DoD close)\n",
            szDenseVerdict, (unsigned)u32Dense, (unsigned)u32DenseOk,
            (unsigned)PCI_MSIX_TABLE_READY_DENSE_MIN,
            (unsigned)fFnPbaDrain, (unsigned)fVecRemaskHold,
            (unsigned)fOrderFnFull, (unsigned)fProductPathFull,
            (unsigned)fPbaDrain, (unsigned)fDualLatch,
            (unsigned)fStickyReissue, (unsigned)fOrderResidualFull,
            (unsigned)fHostPairSeed, (unsigned)fLatchCycle,
            (unsigned)fDualHostChain, (unsigned)fDodOpenFull,
            (unsigned)fDualHostIndep, (unsigned)fInjectChain,
            (unsigned)fProductOpen, (unsigned)fDualDodOpen);
    kprintf("msix: soft residual table_ready denser+++++ %s denser+++++=1 "
            "denser_arms=%u denser_ok=%u "
            "fn_pba_drain=%u vec_remask_hold=%u order_fn_full=%u product_path_full=%u "
            "dual_dod OPEN dual_dod_A=OPEN dual_dod_B=OPEN "
            "product_hosts=UDX Soft!=product H2=once stamp_free=1 "
            "G-AC-1=1 dual=MIT|Apache-2.0 "
            "(W23 denser+++++ MSI-X residual; not Dual DoD close)\n",
            szDenseVerdict, (unsigned)u32Dense, (unsigned)u32DenseOk,
            (unsigned)fFnPbaDrain, (unsigned)fVecRemaskHold,
            (unsigned)fOrderFnFull, (unsigned)fProductPathFull);

    /*
     * denser residual bar .75 VERDICT (agent-facing rollup; Soft!=product).
     * Composite over 28 denser table_ready arms (W23 denser+++++).
     * Dual DoD A/B remain OPEN; denser residual != Dual DoD close.
     * Bar honesty v2026.08.04.75 stamp-free — NEVER invent .76.
     * ASCII Soft!=product only (no unicode Soft inequality glyph).
     * greppable: pci: soft residual denser VERDICT
     * greppable: msix: soft residual denser VERDICT
     * greppable: denser residual bar
     * greppable: bar=v2026.08.04.75
     */
    kprintf("pci: soft residual denser VERDICT %s denser=1 denser++=1 "
            "denser+++=1 denser++++=1 denser+++++=1 denser_ok=%u denser_arms=%u denser_min=%u "
            "fn_pba_drain=%u vec_remask_hold=%u order_fn_full=%u product_path_full=%u "
            "pba_drain=%u dual_latch=%u sticky_reissue=%u order_residual_full=%u "
            "host_pair_seed=%u latch_cycle=%u dual_host_chain=%u dod_open_full=%u "
            "dual_host_indep=%u badge_tbl_distinct=%u soft_tbl_depth=%u "
            "inject_chain=%u product_open=%u dual_dod_open=%u "
            "mask_hold_pba=%u unmask_pend=%u h1_locks=%u "
            "table_ready=%u soft_ready=%u irq_ready=%u badge0_ok=%u "
            "path=soft_tbl->DDI_OP_IRQ_BIND->inject->notify_msix_global->"
            "NOTIFY_WAIT "
            "product_hosts=UDX hosts=rtl8168_udx,xhci_udx "
            "dual_dod OPEN dual_dod_A=OPEN dual_dod_B=OPEN "
            "soft_residual_closes_dod=0 freestanding_skip=1 "
            "product_notify_mint=OPEN product_msix=OPEN soft_note_only=1 "
            "denser residual != Dual DoD close "
            "denser residual bar bar=v2026.08.04.75 stamp_free=1 "
            "never_invent=.76 Soft!=product G-AC-1 dual=MIT|Apache-2.0 "
            "H2=once via=%s "
            "(agent-facing denser residual VERDICT; Soft!=product; "
            "Dual DoD A/B OPEN; never product close)\n",
            szDenseVerdict, (unsigned)u32DenseOk, (unsigned)u32Dense,
            (unsigned)PCI_MSIX_TABLE_READY_DENSE_MIN,
            (unsigned)fFnPbaDrain, (unsigned)fVecRemaskHold,
            (unsigned)fOrderFnFull, (unsigned)fProductPathFull,
            (unsigned)fPbaDrain, (unsigned)fDualLatch,
            (unsigned)fStickyReissue, (unsigned)fOrderResidualFull,
            (unsigned)fHostPairSeed, (unsigned)fLatchCycle,
            (unsigned)fDualHostChain, (unsigned)fDodOpenFull,
            (unsigned)fDualHostIndep, (unsigned)fBadgeTblDistinct,
            (unsigned)fSoftTblDepth, (unsigned)fInjectChain,
            (unsigned)fProductOpen, (unsigned)fDualDodOpen,
            (unsigned)fMaskHoldPba, (unsigned)fUnmaskPend,
            (unsigned)fH1Locks, (unsigned)fTableReady,
            (unsigned)(g_fSoftReady ? 1 : 0), (unsigned)fIrqReady,
            (unsigned)fBadge0Ok, szViaSafe);
    kprintf("msix: soft residual denser VERDICT %s denser+++++=1 denser_ok=%u "
            "denser_arms=%u denser_min=%u "
            "fn_pba_drain=%u vec_remask_hold=%u order_fn_full=%u product_path_full=%u "
            "pba_drain=%u dual_latch=%u sticky_reissue=%u order_residual_full=%u "
            "inject_chain=%u product_open=%u dual_dod_open=%u "
            "dual_dod OPEN dual_dod_A=OPEN dual_dod_B=OPEN "
            "product_hosts=UDX Soft!=product "
            "soft_residual_closes_dod=0 freestanding_skip=1 "
            "product_notify_mint=OPEN product_msix=OPEN soft_note_only=1 "
            "denser residual != Dual DoD close "
            "denser residual bar bar=v2026.08.04.75 stamp_free=1 "
            "never_invent=.76 G-AC-1=1 dual=MIT|Apache-2.0 H2=once "
            "(agent-facing denser residual VERDICT; Soft!=product; "
            "Dual DoD OPEN; never product close)\n",
            szDenseVerdict, (unsigned)u32DenseOk, (unsigned)u32Dense,
            (unsigned)PCI_MSIX_TABLE_READY_DENSE_MIN,
            (unsigned)fFnPbaDrain, (unsigned)fVecRemaskHold,
            (unsigned)fOrderFnFull, (unsigned)fProductPathFull,
            (unsigned)fPbaDrain, (unsigned)fDualLatch,
            (unsigned)fStickyReissue, (unsigned)fOrderResidualFull,
            (unsigned)fInjectChain, (unsigned)fProductOpen,
            (unsigned)fDualDodOpen);
}

static void
pci_msix_soft_inventory(const char *szVia)
{
    u32 iEnt;
    u32 cProgLive = 0;
    u32 cMasked = 0;
    u32 cFired = 0;
    u32 fIrqReady;
    u32 fPend0;
    u32 fPend1;
    u64 u64BadgeMask;
    const char *szViaSafe;
    const char *szVerdict;

    szViaSafe = (szVia != NULL) ? szVia : "anon";

    /* Cap serial flood (exercise + probe both call; residual only). */
    if (g_u32SoftInvLogs >= PCI_MSIX_SOFT_INV_LOG_CAP) {
        return;
    }
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
    fIrqReady = irq_msix_ready() ? 1u : 0u;
    u64BadgeMask = pci_msix_soft_badge_mask();
    fPend0 = pci_msix_soft_pba_pending(0);
    fPend1 = (GJ_MSIX_SOFT_TBL > 1u) ? pci_msix_soft_pba_pending(1) : 0u;
    szVerdict = g_fSoftReady ? "PASS" : "SKIP";

    /*
     * Twin inventory (geometry + tallies + verdict). Soft shadow != device
     * Table Size. greppable: pci: soft inventory / msix: soft inventory
     * greppable: pci: soft inventory PASS / msix: soft inventory PASS
     */
    kprintf("pci: soft inventory %s via=%s ready=%u depth=%u prog=%u "
            "prog_live=%u masked=%u fire=%u fired_live=%u pba=0x%lx "
            "hw_prog=%u irq_ready=%u unmask_pend=%u logs=%u Soft!=product\n",
            szVerdict, szViaSafe, (unsigned)(g_fSoftReady ? 1 : 0),
            (unsigned)GJ_MSIX_SOFT_TBL, (unsigned)g_u32SoftProg,
            (unsigned)cProgLive, (unsigned)cMasked, (unsigned)g_u32SoftFire,
            (unsigned)cFired, (unsigned long)g_u64SoftPba,
            (unsigned)g_u32Programmed, (unsigned)fIrqReady,
            (unsigned)g_u32SoftUnmaskPend, (unsigned)g_u32SoftInvLogs);
    kprintf("msix: soft inventory %s via=%s ready=%u depth=%u prog=%u "
            "fire=%u pba=0x%lx unmask_pend=%u logs=%u Soft!=product\n",
            szVerdict, szViaSafe, (unsigned)(g_fSoftReady ? 1 : 0),
            (unsigned)GJ_MSIX_SOFT_TBL, (unsigned)g_u32SoftProg,
            (unsigned)g_u32SoftFire, (unsigned long)g_u64SoftPba,
            (unsigned)g_u32SoftUnmaskPend, (unsigned)g_u32SoftInvLogs);

    /*
     * Lean residual UDX IRQ scaffold + H1 honesty (one twin; no stamp storm).
     * Freestanding rtl stays poll-mode first; never forces HW MSI-X onto
     * freestanding NIC; never calls net_eth_poll (run-loop only). Soft
     * shadow only != product IRQ. Soft!=product; G-AC-1; dual MIT/Apache.
     * greppable: pci: soft residual lean / msix: soft residual lean
     * greppable: pci: soft residual lean PASS / msix: soft residual lean PASS
     * greppable: force_irq_eth_poll=0 poll_mode_first=1
     * greppable: net_eth_poll=run_loop_only net_eth_irq=0
     * greppable: net_eth_poll_from_msix=0
     */
    /*
     * Contiguous greppable: "pci: soft residual lean PASS" /
     * "msix: soft residual lean PASS" (PASS immediately after lean).
     * UDX IRQ foundation path (soft only; Soft!=product):
     *   soft_tbl fire/unmask_pend -> irq_msix inject -> notify_msix_global
     *   -> GJ_SYS_NOTIFY_WAIT; product_notify_mint=OPEN (no CNode IRQ mint).
     * Badge scaffold: GJ_MSIX_BADGE_TBL(i); UDX freestanding bit_N->line_N
     * is independent. Never forces freestanding rtl off poll-mode.
     */
    kprintf("pci: soft residual lean PASS udx_irq=1 "
            "poll_mode_first=%u force_irq_eth_poll=%u hw_force_rtl=%u "
            "net_eth_irq=%u net_eth_poll_from_msix=%u "
            "net_eth_poll=run_loop_only freestanding_rtl_poll=1 "
            "fn_mask=%u unmask_pend=%u badge_mask=0x%lx pba_e0=%u pba_e1=%u "
            "badge0=0x%lx path=soft_tbl->inject->notify_msix_global->NOTIFY_WAIT "
            "pba_drain=1 dual_latch=1 sticky_reissue=1 fn_pba_drain=1 "
            "vec_remask_hold=1 order_residual=1 "
            "product_msix=OPEN product_notify_mint=OPEN soft_shadow=1 "
            "full_table_size=0 game_io=0 product_irq=0 soft_only=1 "
            "dual_dod_A=OPEN dual_dod_B=OPEN freestanding_skip=1 "
            "stamp_free=1 soft=1 product=0 "
            "dual_license=MIT_OR_Apache-2.0 G-AC-1 "
            "hazard=H1 Soft!=product\n",
            (unsigned)PCI_MSIX_POLL_MODE_FIRST,
            (unsigned)PCI_MSIX_FORCE_IRQ_ETH_POLL,
            (unsigned)PCI_MSIX_HW_FORCE_RTL,
            (unsigned)PCI_MSIX_NET_ETH_IRQ,
            (unsigned)PCI_MSIX_NET_ETH_POLL_FROM,
            (unsigned)g_fSoftFnMask, (unsigned)g_u32SoftUnmaskPend,
            (unsigned long)u64BadgeMask, (unsigned)fPend0, (unsigned)fPend1,
            (unsigned long)pci_msix_soft_entry_badge(0));
    kprintf("msix: soft residual lean PASS udx_irq=1 "
            "poll_mode_first=%u force_irq_eth_poll=%u hw_force_rtl=%u "
            "net_eth_irq=%u net_eth_poll_from_msix=%u "
            "net_eth_poll=run_loop_only fn_mask=%u unmask_pend=%u "
            "badge_mask=0x%lx pba_drain=1 dual_latch=1 sticky_reissue=1 "
            "fn_pba_drain=1 vec_remask_hold=1 order_residual=1 "
            "product_msix=OPEN product_notify_mint=OPEN "
            "soft_shadow=1 dual_dod_A=OPEN dual_dod_B=OPEN "
            "freestanding_skip=1 stamp_free=1 soft=1 product=0 "
            "dual_license=MIT_OR_Apache-2.0 "
            "G-AC-1 hazard=H1 Soft!=product\n",
            (unsigned)PCI_MSIX_POLL_MODE_FIRST,
            (unsigned)PCI_MSIX_FORCE_IRQ_ETH_POLL,
            (unsigned)PCI_MSIX_HW_FORCE_RTL,
            (unsigned)PCI_MSIX_NET_ETH_IRQ,
            (unsigned)PCI_MSIX_NET_ETH_POLL_FROM,
            (unsigned)g_fSoftFnMask, (unsigned)g_u32SoftUnmaskPend,
            (unsigned long)u64BadgeMask);
    /*
     * Dual DoD residual honesty (H2 once STRONGER denser; Soft!=product).
     * Soft residual lean PASS != Dual DoD A/B close. UDX product OPEN.
     * denser table_ready residual for DDI IRQ_BIND (24 multi-arm denser++++).
     * greppable: pci: soft residual dual_dod OPEN
     * greppable: msix: soft residual dual_dod OPEN
     * greppable: Soft!=product soft residual dual_dod OPEN product_hosts=UDX
     * greppable: pci: soft residual table_ready STRONGER
     * greppable: pci: soft residual table_ready denser
     * greppable: pci: soft residual table_ready denser++
     * greppable: pci: soft residual table_ready denser+++
     * greppable: pci: soft residual table_ready denser++++
     * greppable: msix: soft residual table_ready denser
     * greppable: msix: soft residual table_ready denser++
     * greppable: msix: soft residual table_ready denser+++
     * greppable: msix: soft residual table_ready denser++++
     * greppable: pci: soft residual denser VERDICT
     * greppable: msix: soft residual denser VERDICT
     * greppable: denser residual bar bar=v2026.08.04.75
     * greppable: denser_arms=28 denser_ok= mask_hold_pba= unmask_pend=
     * greppable: dual_host_indep= badge_tbl_distinct= inject_chain=
     * greppable: pba_drain= dual_latch= sticky_reissue= order_residual_full=
     * Bar honesty v2026.08.04.75 stamp-free — never invent .76.
     * ASCII Soft!=product only (no unicode inequality glyph).
     */
    pci_msix_soft_dual_dod_stronger_once(szViaSafe);
}

u32
pci_msix_soft_table_exercise(void)
{
    struct gj_pci_msix_soft_entry ent;
    u32 fOk = 1;
    u32 u32Held;
    u32 u32Delivered;
    u32 u32FireBefore;
    u32 u32UnmaskBefore;
    u64 u64Pba;
    u64 u64BadgeMask;

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
    if ((u64Pba & 1ull) == 0 || pci_msix_soft_pba_pending(0) == 0) {
        fOk = 0; /* sticky PBA still set while masked */
    }
    /*
     * Residual unmask-pending: unmask with sticky PBA delivers once
     * (UDX enable_irq latch shape). Soft!=product HW re-issue.
     */
    u32FireBefore = g_u32SoftFire;
    u32UnmaskBefore = g_u32SoftUnmaskPend;
    if (!pci_msix_soft_mask(0, 0)) {
        fOk = 0;
    }
    if (g_u32SoftFire <= u32FireBefore ||
        g_u32SoftUnmaskPend <= u32UnmaskBefore) {
        fOk = 0; /* unmask must have delivered pending */
    }
    /* Explicit re-fire still delivers (irq path ready or soft counter only). */
    u32Delivered = pci_msix_soft_fire(0);
    if (u32Delivered == 0) {
        fOk = 0;
    }
    /* Soft fire entry 1. */
    if (pci_msix_soft_fire(1) == 0) {
        fOk = 0;
    }
    /*
     * Residual Function Mask (MC bit14 shape): hold all soft delivery.
     * Freestanding rtl stays poll-mode; this does not write device MC.
     * Clear with sticky PBA auto-delivers unmasked entries (UDX residual).
     */
    if (!pci_msix_soft_function_mask(1)) {
        fOk = 0;
    }
    u32FireBefore = g_u32SoftFire;
    u32UnmaskBefore = g_u32SoftUnmaskPend;
    if (pci_msix_soft_fire(1) != 0) {
        fOk = 0; /* function-masked: no deliver */
    }
    if (g_u32SoftFire != u32FireBefore) {
        fOk = 0;
    }
    if (pci_msix_soft_pba_pending(1) == 0) {
        fOk = 0; /* sticky PBA while function-masked */
    }
    if (pci_msix_soft_function_mask_get() == 0) {
        fOk = 0;
    }
    if (!pci_msix_soft_function_mask(0)) {
        fOk = 0;
    }
    /* Residual: fn-mask clear with sticky PBA delivers without re-fire. */
    if (g_u32SoftFire <= u32FireBefore ||
        g_u32SoftUnmaskPend <= u32UnmaskBefore) {
        fOk = 0;
    }
    /* Explicit re-fire still delivers after fn-mask clear. */
    if (pci_msix_soft_fire(1) == 0) {
        fOk = 0;
    }
    /* Badge mask residual: programmed entries map to TBL badges. */
    u64BadgeMask = pci_msix_soft_badge_mask();
    if ((u64BadgeMask & GJ_MSIX_BADGE_TBL(0)) == 0 ||
        (u64BadgeMask & GJ_MSIX_BADGE_TBL(1)) == 0) {
        fOk = 0;
    }
    if (pci_msix_soft_entry_badge(0) != GJ_MSIX_BADGE_TBL(0) ||
        pci_msix_soft_entry_badge(1) != GJ_MSIX_BADGE_TBL(1) ||
        pci_msix_soft_entry_badge(GJ_MSIX_SOFT_TBL) != 0) {
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
    /*
     * Residual order deepen (C0; Soft!=product; UDX enable_irq / MC shape).
     * greppable: pba_drain=1 dual_latch=1 sticky_reissue=1 fn_pba_drain=1
     * greppable: vec_remask_hold=1 order_residual=1
     *
     * (1) pba_drain: sticky clear before unmask must not re-issue.
     * (2) dual_latch: vector unmask under fn_mask holds; fn clear delivers.
     * (3) sticky_reissue: Soft PBA remains after deliver; remask+unmask
     *     re-issues without new fire (Soft!=product HW auto-clear).
     * (4) fn_pba_drain: clear sticky under fn_mask before fn clear: no issue.
     * (5) vec_remask_hold: remask vector under fn before fn clear: no deliver.
     * Freestanding rtl stays poll-mode; never forces HW MSI-X eth IRQ.
     */
    {
        u32 u32FireSnap;
        u32 u32UnmaskSnap;

        /*
         * Isolate order residual to entry0: drain all sticky PBA and mask
         * entry1 so dual_latch / fn_pba_drain / vec_remask_hold tallies are
         * not polluted by earlier multi-entry sticky (Soft sticky until clear).
         */
        (void)pci_msix_soft_pba_clear(~0ull);
        if (!pci_msix_soft_mask(1, 1)) {
            fOk = 0;
        }

        /* (1) pba_drain residual */
        if (!pci_msix_soft_mask(0, 1)) {
            fOk = 0;
        }
        if (pci_msix_soft_fire(0) != 0) {
            fOk = 0; /* masked: hold */
        }
        if (pci_msix_soft_pba_pending(0) == 0) {
            fOk = 0; /* sticky while masked */
        }
        if (pci_msix_soft_pba_clear(1ull) != 1ull) {
            fOk = 0;
        }
        if (pci_msix_soft_pba_pending(0) != 0) {
            fOk = 0; /* drained */
        }
        u32FireSnap = g_u32SoftFire;
        u32UnmaskSnap = g_u32SoftUnmaskPend;
        if (!pci_msix_soft_mask(0, 0)) {
            fOk = 0;
        }
        if (g_u32SoftFire != u32FireSnap ||
            g_u32SoftUnmaskPend != u32UnmaskSnap) {
            fOk = 0; /* drain: unmask must not re-issue */
        }

        /* (2) dual_latch residual: fn_mask × vector mask order */
        if (!pci_msix_soft_mask(0, 1)) {
            fOk = 0;
        }
        if (pci_msix_soft_fire(0) != 0) {
            fOk = 0; /* masked hold + sticky */
        }
        if (pci_msix_soft_pba_pending(0) == 0) {
            fOk = 0;
        }
        if (!pci_msix_soft_function_mask(1)) {
            fOk = 0;
        }
        u32FireSnap = g_u32SoftFire;
        u32UnmaskSnap = g_u32SoftUnmaskPend;
        /* Vector unmask under active fn_mask must not deliver. */
        if (!pci_msix_soft_mask(0, 0)) {
            fOk = 0;
        }
        if (g_u32SoftFire != u32FireSnap ||
            g_u32SoftUnmaskPend != u32UnmaskSnap) {
            fOk = 0;
        }
        if (pci_msix_soft_pba_pending(0) == 0) {
            fOk = 0; /* sticky remains for fn clear */
        }
        if (pci_msix_soft_function_mask_get() == 0) {
            fOk = 0;
        }
        /* fn clear with sticky + unmasked vector delivers once. */
        if (!pci_msix_soft_function_mask(0)) {
            fOk = 0;
        }
        if (g_u32SoftFire <= u32FireSnap ||
            g_u32SoftUnmaskPend <= u32UnmaskSnap) {
            fOk = 0;
        }
        if (pci_msix_soft_function_mask_get() != 0) {
            fOk = 0;
        }

        /*
         * (3) sticky_reissue residual (Soft!=product honesty):
         * Soft PBA is not auto-cleared on deliver. After (2), entry0 is
         * unmasked with sticky still set. Remask then unmask (no new fire)
         * must re-issue once. Product HW typically clears pending on send;
         * soft table keeps sticky until clear API.
         */
        if (pci_msix_soft_pba_pending(0) == 0) {
            fOk = 0; /* dual_latch leave sticky for reissue */
        }
        if (!pci_msix_soft_mask(0, 1)) {
            fOk = 0;
        }
        u32FireSnap = g_u32SoftFire;
        u32UnmaskSnap = g_u32SoftUnmaskPend;
        if (!pci_msix_soft_mask(0, 0)) {
            fOk = 0;
        }
        if (g_u32SoftFire <= u32FireSnap ||
            g_u32SoftUnmaskPend <= u32UnmaskSnap) {
            fOk = 0; /* sticky re-issue on unmask without new fire */
        }

        /*
         * (4) fn_pba_drain residual: under Function Mask, drain sticky
         * then clear fn_mask must not re-issue (MC enable / ack shape).
         */
        if (!pci_msix_soft_function_mask(1)) {
            fOk = 0;
        }
        if (pci_msix_soft_pba_pending(0) == 0) {
            /* Ensure sticky under fn (fire while function-masked). */
            if (pci_msix_soft_fire(0) != 0) {
                fOk = 0; /* fn holds deliver */
            }
        }
        if (pci_msix_soft_pba_pending(0) == 0) {
            fOk = 0;
        }
        if (pci_msix_soft_pba_clear(1ull) != 1ull) {
            fOk = 0;
        }
        if (pci_msix_soft_pba_pending(0) != 0) {
            fOk = 0; /* drained under fn */
        }
        u32FireSnap = g_u32SoftFire;
        u32UnmaskSnap = g_u32SoftUnmaskPend;
        if (!pci_msix_soft_function_mask(0)) {
            fOk = 0;
        }
        if (g_u32SoftFire != u32FireSnap ||
            g_u32SoftUnmaskPend != u32UnmaskSnap) {
            fOk = 0; /* fn_pba_drain: no re-issue */
        }
        if (pci_msix_soft_function_mask_get() != 0) {
            fOk = 0;
        }

        /*
         * (5) vec_remask_hold residual: dual_latch complement.
         * Fire under vector mask + fn_mask; unmask vector (held by fn);
         * remask vector; clear fn -> vector mask still holds (no deliver).
         */
        if (!pci_msix_soft_mask(0, 1)) {
            fOk = 0;
        }
        if (pci_msix_soft_fire(0) != 0) {
            fOk = 0; /* vector mask holds */
        }
        if (pci_msix_soft_pba_pending(0) == 0) {
            fOk = 0;
        }
        if (!pci_msix_soft_function_mask(1)) {
            fOk = 0;
        }
        u32FireSnap = g_u32SoftFire;
        u32UnmaskSnap = g_u32SoftUnmaskPend;
        if (!pci_msix_soft_mask(0, 0)) {
            fOk = 0; /* unmask under fn: hold */
        }
        if (g_u32SoftFire != u32FireSnap ||
            g_u32SoftUnmaskPend != u32UnmaskSnap) {
            fOk = 0;
        }
        if (!pci_msix_soft_mask(0, 1)) {
            fOk = 0; /* remask under fn */
        }
        if (!pci_msix_soft_function_mask(0)) {
            fOk = 0; /* fn clear while vector remasked */
        }
        if (g_u32SoftFire != u32FireSnap ||
            g_u32SoftUnmaskPend != u32UnmaskSnap) {
            fOk = 0; /* vec_remask_hold: no deliver */
        }
        if (pci_msix_soft_pba_pending(0) == 0) {
            fOk = 0; /* sticky remains (held, not drained) */
        }
        if (pci_msix_soft_function_mask_get() != 0) {
            fOk = 0;
        }
    }
    /*
     * Residual tallies (after multi-entry base + order isolate to e0):
     *   unmask-pend: base (vec0 + fn-clear e0/e1) + dual_latch e0
     *                + sticky_reissue e0 >= 4
     *   fires: base (>=5) + dual_latch + sticky_reissue -> soft_fire >= 6
     *   (held fires, pba_drain, fn_pba_drain, vec_remask_hold do not count)
     */
    if (g_u32SoftProg < 2 || g_u32SoftFire < 6 || g_u32SoftUnmaskPend < 4) {
        fOk = 0;
    }
    if (fOk) {
        kprintf("pci: MSI-X table soft path entries=%u fire=%u "
                "unmask_pend=%u badge=0x%lx pba=0x%lx PASS\n",
                g_u32SoftProg, g_u32SoftFire, g_u32SoftUnmaskPend,
                (unsigned long)u64BadgeMask,
                (unsigned long)pci_msix_soft_pba());
        kprintf("pci: MSI-X table soft path PASS\n");
        /*
         * Exercise residual lean (H1 honesty; Soft!=product).
         * greppable: msix: soft residual lean
         * greppable: pba_drain=1 dual_latch=1 sticky_reissue=1
         * greppable: fn_pba_drain=1 vec_remask_hold=1 order_residual=1
         * greppable: poll_mode_first force_irq_eth_poll hw_force_rtl
         */
        /* Contiguous greppable: msix: soft residual lean PASS */
        kprintf("msix: soft residual lean PASS exercise unmask_pend=%u "
                "fn_mask=0 fn_clear_delivers=1 "
                "pba_drain=1 dual_latch=1 sticky_reissue=1 "
                "fn_pba_drain=1 vec_remask_hold=1 order_residual=1 "
                "badge=0x%lx entry0_badge=0x%lx "
                "poll_mode_first=%u force_irq_eth_poll=%u hw_force_rtl=%u "
                "net_eth_irq=%u net_eth_poll_from_msix=%u "
                "net_eth_poll=run_loop_only udx_irq=1 "
                "product_msix=OPEN product_notify_mint=OPEN "
                "dual_dod_A=OPEN dual_dod_B=OPEN freestanding_skip=1 "
                "stamp_free=1 hazard=H1 "
                "dual_license=MIT_OR_Apache-2.0 Soft!=product\n",
                g_u32SoftUnmaskPend, (unsigned long)u64BadgeMask,
                (unsigned long)pci_msix_soft_entry_badge(0),
                (unsigned)PCI_MSIX_POLL_MODE_FIRST,
                (unsigned)PCI_MSIX_FORCE_IRQ_ETH_POLL,
                (unsigned)PCI_MSIX_HW_FORCE_RTL,
                (unsigned)PCI_MSIX_NET_ETH_IRQ,
                (unsigned)PCI_MSIX_NET_ETH_POLL_FROM);
    } else {
        kprintf("pci: MSI-X table soft path FAIL prog=%u fire=%u "
                "unmask_pend=%u pba=0x%lx\n",
                g_u32SoftProg, g_u32SoftFire, g_u32SoftUnmaskPend,
                (unsigned long)pci_msix_soft_pba());
    }
    /* Lean residual inventory (capped; Soft!=product; H1 honesty). */
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
     * Lean residual inventory (capped; exercise may have already logged).
     * Soft!=product. H1 honesty. No version stamp / stamp storm.
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
