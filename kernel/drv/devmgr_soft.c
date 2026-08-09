/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft DDI / devmgr inventory for the ABI-first pivot.
 * Pure C11 freestanding. Dual MIT OR Apache-2.0. No GPL / no Linux .ko.
 *
 * Soft only:
 *   - init lamp (devmgr: soft init PASS) - idempotent; may defer bus walk
 *   - Real type-0 PCI config walk via CF8/CFC (inline helpers in this TU)
 *   - Soft PCI inventory: BDF, vendor:device, class/subclass/prog-if, BAR0..5
 *   - BAR size via classic write-all-1s probe; BAR restored after probe
 *   - Residual deepen (real CF8 snapshot for DDI / G752 dual-DoD seeds):
 *       rev, IRQ line/pin, cmd/status, subsystem, cap ptr, MF, BAR mem/io N
 *       dual-DoD class_ok + match_graph=OPEN; soft window seed on HIT
 *       product-host preferred BAR geom (rtl BAR0+BAR2; xhci BAR0)
 *       cmd enable residual (IO/MEM/BM bits; soft observe; not pci_enable)
 *   - DMA window grant note (soft table; no live cap mint / VT-d program)
 *   - Soft helpers: bdf_register, find_by_id, index_by_id, dual_dod_hits,
 *     fn_first_mem_bar, count, get, note_window_grant
 *
 * Lean soft residual (this TU exclusive; Soft!=product / G-AC-1 / C0 / W4):
 *   - Once-lamp dual-DoD HIT/MISS + first-MEM BAR geom (pa/cb/is64/pref)
 *   - Preferred BAR residual (UDX product hosts): rtl0+rtl2, xhci0 + pref_ok
 *   - Soft inventory idx + class_ok (+ xHCI progif_ok) + hits_ok self-check
 *   - cmd_io/mem/bm residual bits on dual-DoD HITs (seed only; not product)
 *   - Soft inventory seeds DDI door / UDX product path only (not class own)
 *   - Path honesty: inventory -> find -> window_note -> ddi_door
 *     SCAN/GET/OPEN/MAP_BAR -> UDX hosts (rtl8168_udx / xhci_udx) -> mint OPEN
 *   - Match graph product remains OPEN (soft residual != product match)
 *   - Cap kprintf hard: init / scan PASS / found HIT|MISS / window note /
 *     residual lean / residual deepen - once-lamps only; NO stamp storms;
 *     NO version stamps (BAR v2026.08.04.75 stamp-free; never bump version)
 *
 * Product direction (docs/DDI_SOFT.md, UDX_LINUX_PORTER.md, ABI_FIRST_PIVOT):
 *   Linux-shaped userspace hosts over DDI/UDX + MMIO/IRQ/DMA caps.
 *   Freestanding class drivers (rtl8168.c / xhci_msc) are residual SKIP only
 *   - not product NIC/USB; default probe off. Soft residual never rewrites
 *   that product bar. G-AC-1: no Linux .ko product AC.
 *   Product hosts = UDX. Match graph = OPEN. Dual DoD A/B remain OPEN.
 *
 * Dual-DoD soft inventory seeds (G752VT HIT or MISS after first pci_scan):
 *   Realtek NIC  10ec:8168  - dual DoD B seed (rtl8168_udx); not TX/RX
 *     preferred BARs: BAR0 (DDI grant path) + BAR2 (silicon-common MMIO)
 *   Intel xHCI   8086:a12f  - dual DoD A seed (xhci_udx); not BOT/MSC
 *     preferred BAR: BAR0
 *
 * Soft!=product always. Inventory look only - not TX/RX, not BOT/MSC,
 * not sole BAR owner, not freestanding product claim, not .ko product.
 *
 * Greppable markers (keep stable):
 *   devmgr: soft init PASS
 *   devmgr: soft pci scan PASS n=N
 *   devmgr: soft found 10ec:8168   (HIT | MISS once-lamp)
 *   devmgr: soft found 8086:a12f   (HIT | MISS once-lamp)
 *   devmgr: soft dma window note
 *   devmgr: soft residual lean
 *   devmgr: soft pci scan residual deepen
 *   Soft!=product  (honesty tag on soft lamps)
 */
#include <gj/devmgr.h>
#include <gj/klog.h>
#include <gj/string.h>
#include <gj/types.h>

/* ---- PCI config (type-0 mechanism; inline in this TU only) ------------ */
#define DEVMGR_PCI_CFG_ADDR 0xCF8u
#define DEVMGR_PCI_CFG_DATA 0xCFCu
#define DEVMGR_PCI_BUS_MAX  8u  /* buses 0..7 */
#define DEVMGR_PCI_SLOT_MAX 32u /* slots 0..31 */
#define DEVMGR_PCI_FUNC_MAX 8u  /* funcs 0..7 */

/* G752VT greppable target IDs (aliases of public header macros). */
#define DEVMGR_VID_REALTEK GJ_DEVMGR_VID_REALTEK
#define DEVMGR_DID_RTL8168 GJ_DEVMGR_DID_RTL8168
#define DEVMGR_VID_INTEL   GJ_DEVMGR_VID_INTEL
#define DEVMGR_DID_XHCI    GJ_DEVMGR_DID_XHCI_A12F

/*
 * Product-host preferred BAR residual (mirror UDX/DDI catalog; Soft!=product).
 * rtl8168_udx: BAR0 (DDI grant path) + BAR2 (silicon-common MMIO).
 * xhci_udx:    BAR0 only.
 * Soft inventory look only - not sole BAR owner; mint remains OPEN.
 */
#define DEVMGR_SOFT_RTL_PREF_BAR0   0u
#define DEVMGR_SOFT_RTL_PREF_BAR2   2u
#define DEVMGR_SOFT_XHCI_PREF_BAR0  0u

/* PCI command residual bits (config 0x04 low). Soft observe; not pci_enable. */
#define DEVMGR_SOFT_CMD_IO   0x1u
#define DEVMGR_SOFT_CMD_MEM  0x2u
#define DEVMGR_SOFT_CMD_BM   0x4u

static int  g_fReady;
static int  g_fScanned;
static u32  g_cInitCalls;
static u32  g_cPciScanCalls;
static u32  g_cWinNotes;
static u32  g_cWinRejects;
static u32  g_cWinUsed;
static u32  g_cBdfUsed;
static u32  g_cBdfRegs;
static u32  g_cBdfRejects;
static u32  g_cPciUsed;
static u32  g_u32DualDodHits; /* soft match residual bits after first fill */
static u32  g_cDualDodWinSeed; /* soft window seeds for dual-DoD preferred MEM */

/* Once-lamps only - NO stamp storms, NO version stamps. Soft!=product. */
static u8   g_fInitLampOnce;
static u8   g_fScanLampOnce;
static u8   g_fFoundLampOnce;
static u8   g_fWinNoteLampOnce;
static u8   g_fWinRejectLampOnce;
static u8   g_fResidualOnce;
static u8   g_fScanDeepenOnce;

/* Soft window note table (inventory only). */
static struct gj_devmgr_window_grant g_aWin[GJ_DEVMGR_SOFT_WIN_MAX];

/* Soft BDF registration table (manual inventory; no live PCI). */
static struct gj_devmgr_bdf g_aBdf[GJ_DEVMGR_SOFT_BDF_MAX];
static u8                   g_aBdfOk[GJ_DEVMGR_SOFT_BDF_MAX];

/* Soft PCI function inventory from CF8/CFC walk. */
static struct gj_devmgr_pci_fn g_aPci[GJ_DEVMGR_SOFT_BDF_MAX];

/* Forward: dual-DoD window seed residual calls this before body below. */
int devmgr_soft_dma_window_note(const struct gj_devmgr_bdf *pBdf,
                                u64 u64PaBase, u64 u64Cb, u8 u8Rw,
                                struct gj_devmgr_window_grant *pGrantOut);

/**
 * First MEM BAR (u8Mem!=0, non-zero size) on a soft inventory function.
 * Returns bar index or -1. Soft inventory snapshot only - not sole BAR
 * owner and not freestanding product claim. Product BAR/MMIO ownership
 * is UDX/DDI host grant (mint OPEN).
 */
int
devmgr_soft_fn_first_mem_bar(const struct gj_devmgr_pci_fn *pFn)
{
    u8 i;

    if (pFn == NULL) {
        return -1;
    }
    for (i = 0u; i < GJ_DEVMGR_SOFT_BAR_MAX; i++) {
        if (pFn->aBar[i].u8Mem != 0u && pFn->aBar[i].u64Cb != 0ull) {
            return (int)i;
        }
    }
    return -1;
}

/**
 * Soft residual: non-zero if BAR u8Idx is MEM with non-zero size.
 * Preferred-BAR honesty seed for UDX product hosts (Soft!=product).
 * Not sole BAR owner; not product MAP_BAR grant.
 */
static int
devmgr_soft_bar_mem_ok(const struct gj_devmgr_pci_fn *pFn, u8 u8Idx)
{
    if (pFn == NULL || u8Idx >= GJ_DEVMGR_SOFT_BAR_MAX) {
        return 0;
    }
    if (pFn->aBar[u8Idx].u8Mem != 0u && pFn->aBar[u8Idx].u64Cb != 0ull) {
        return 1;
    }
    return 0;
}

/**
 * Soft residual: seed one DMA window note from a specific MEM BAR index.
 * Returns 1 if note accepted, 0 if skipped/rejected. Soft table only.
 */
static int
devmgr_soft_seed_mem_bar_window(const struct gj_devmgr_pci_fn *pFn, u8 u8Idx)
{
    if (devmgr_soft_bar_mem_ok(pFn, u8Idx) == 0) {
        return 0;
    }
    if (devmgr_soft_dma_window_note(&pFn->bdf,
                                    pFn->aBar[u8Idx].u64Pa,
                                    pFn->aBar[u8Idx].u64Cb,
                                    0x3u, NULL) == 0) {
        return 1;
    }
    return 0;
}

/**
 * Soft dual-DoD class_ok residual (inventory honesty only).
 * RTL8168: class=0x02 (net) + subclass=0x00 (eth) required for class_ok.
 * xHCI a12f: class=0x0c subclass=0x03; progif 0x30 preferred (not req).
 * Soft residual - not product match graph (graph remains OPEN).
 */
static int
devmgr_soft_class_ok_rtl(const struct gj_devmgr_pci_fn *pFn)
{
    if (pFn == NULL) {
        return 0;
    }
    if (pFn->u8Class != GJ_DEVMGR_CLASS_NET) {
        return 0;
    }
    /* subclass ETH required for dual-DoD class_ok residual seed */
    if (pFn->u8Subclass != GJ_DEVMGR_SUBCLASS_ETH) {
        return 0;
    }
    return 1;
}

static int
devmgr_soft_class_ok_xhci(const struct gj_devmgr_pci_fn *pFn)
{
    if (pFn == NULL) {
        return 0;
    }
    if (pFn->u8Class != GJ_DEVMGR_CLASS_SERIAL) {
        return 0;
    }
    if (pFn->u8Subclass != GJ_DEVMGR_SUBCLASS_USB) {
        return 0;
    }
    /* progif 0x30 = xHCI preferred; still soft-HIT on class/subclass alone */
    (void)pFn->u8ProgIf;
    return 1;
}

/** Soft residual: non-zero if progif matches xHCI (0x30). Soft!=product. */
static int
devmgr_soft_progif_ok_xhci(const struct gj_devmgr_pci_fn *pFn)
{
    if (pFn == NULL) {
        return 0;
    }
    return (pFn->u8ProgIf == GJ_DEVMGR_PROGIF_XHCI) ? 1 : 0;
}

/**
 * Soft dual-DoD window seed residual (preferred MEM BARs of each HIT).
 * Product-host preferred seed (Soft!=product; no live VT-d / no cap mint):
 *   rtl8168_udx (DoD B): BAR0 + BAR2 when each is MEM with size
 *   xhci_udx    (DoD A): BAR0 when MEM with size
 * Fallback: first MEM BAR if no preferred BAR seeded (inventory honesty).
 * Once per scan fill. Seeds DDI DMA_NOTE path only. Dual DoD A/B remain OPEN.
 */
static void
devmgr_soft_dual_dod_window_seed(void)
{
    u32 i;
    int iBar;
    int cSeeded;

    for (i = 0u; i < g_cPciUsed; i++) {
        const struct gj_devmgr_pci_fn *pFn = &g_aPci[i];

        cSeeded = 0;

        if (pFn->u16Vendor == DEVMGR_VID_REALTEK &&
            pFn->u16Device == DEVMGR_DID_RTL8168) {
            /* Prefer BAR0 (DDI grant) + BAR2 (silicon-common MMIO). */
            if (devmgr_soft_seed_mem_bar_window(pFn,
                                               DEVMGR_SOFT_RTL_PREF_BAR0) != 0) {
                g_cDualDodWinSeed++;
                cSeeded++;
            }
            if (devmgr_soft_seed_mem_bar_window(pFn,
                                               DEVMGR_SOFT_RTL_PREF_BAR2) != 0) {
                g_cDualDodWinSeed++;
                cSeeded++;
            }
        } else if (pFn->u16Vendor == DEVMGR_VID_INTEL &&
                   pFn->u16Device == DEVMGR_DID_XHCI) {
            /* Prefer BAR0 (xhci_udx MAP path). */
            if (devmgr_soft_seed_mem_bar_window(pFn,
                                               DEVMGR_SOFT_XHCI_PREF_BAR0) != 0) {
                g_cDualDodWinSeed++;
                cSeeded++;
            }
        } else {
            continue;
        }

        if (cSeeded != 0) {
            continue;
        }

        /* Fallback: first MEM BAR residual (no preferred MEM present). */
        iBar = devmgr_soft_fn_first_mem_bar(pFn);
        if (iBar < 0) {
            continue;
        }
        if (pFn->aBar[iBar].u64Cb == 0ull) {
            continue;
        }
        /* Soft RW=0x3 residual seed; reject/full counted by note helper. */
        if (devmgr_soft_dma_window_note(&pFn->bdf,
                                        pFn->aBar[iBar].u64Pa,
                                        pFn->aBar[iBar].u64Cb,
                                        0x3u, NULL) == 0) {
            g_cDualDodWinSeed++;
        }
    }
}

/**
 * Lean dual-DoD residual once-lamp (C0 soft inventory honesty; W4 strengthen).
 *
 * Single line after first CF8 fill (or first window note):
 *   dual-DoD HIT/MISS for 10ec:8168 + 8086:a12f
 *   first-MEM BAR geom + is64/pref when HIT (snapshot seed for DDI MAP_BAR)
 *   preferred BAR residual (rtl0+rtl2, xhci0) + pref_ok self-check
 *   cmd_io/mem/bm residual bits on dual-DoD HITs (soft observe only)
 *   soft inventory idx (ddi_door OPEN seed) + class_ok + win_seed
 *   counters + DDI/UDX product path honesty (mint OPEN; match graph OPEN)
 *
 * Soft!=product / G-AC-1. Soft lamps never close Dual DoD A/B.
 * No version stamp. No stamp storms.
 * greppable: devmgr: soft residual lean
 */
static void
devmgr_soft_residual_lean_once(void)
{
    u32 i;
    int fRtl = 0;
    int fXhci = 0;
    int iRtl = -1;
    int iXhci = -1;
    int iRtlBar = -1;
    int iXhciBar = -1;
    int fRtlClassOk = 0;
    int fXhciClassOk = 0;
    int fXhciProgIfOk = 0;
    int fRtlPref0 = 0;
    int fRtlPref2 = 0;
    int fXhciPref0 = 0;
    int fPrefOk = 0;
    int fRtlIo = 0;
    int fRtlMem = 0;
    int fRtlBm = 0;
    int fXhciIo = 0;
    int fXhciMem = 0;
    int fXhciBm = 0;
    u8  u8RtlIs64 = 0u;
    u8  u8RtlPref = 0u;
    u8  u8XhciIs64 = 0u;
    u8  u8XhciPref = 0u;
    u16 u16RtlCmd = 0u;
    u16 u16XhciCmd = 0u;
    u64 u64RtlPa = 0;
    u64 u64RtlCb = 0;
    u64 u64RtlPa0 = 0;
    u64 u64RtlCb0 = 0;
    u64 u64RtlPa2 = 0;
    u64 u64RtlCb2 = 0;
    u64 u64XhciPa = 0;
    u64 u64XhciCb = 0;
    u64 u64XhciPa0 = 0;
    u64 u64XhciCb0 = 0;
    u32 u32HitsExpect = 0u;
    int fHitsOk = 0;

    if (g_fResidualOnce != 0u) {
        return;
    }
    g_fResidualOnce = 1u;

    for (i = 0u; i < g_cPciUsed; i++) {
        if (!fRtl && g_aPci[i].u16Vendor == DEVMGR_VID_REALTEK &&
            g_aPci[i].u16Device == DEVMGR_DID_RTL8168) {
            fRtl = 1;
            iRtl = (int)i;
            fRtlClassOk = devmgr_soft_class_ok_rtl(&g_aPci[i]);
            iRtlBar = devmgr_soft_fn_first_mem_bar(&g_aPci[i]);
            u16RtlCmd = g_aPci[i].u16Cmd;
            fRtlIo = ((u16RtlCmd & DEVMGR_SOFT_CMD_IO) != 0u) ? 1 : 0;
            fRtlMem = ((u16RtlCmd & DEVMGR_SOFT_CMD_MEM) != 0u) ? 1 : 0;
            fRtlBm = ((u16RtlCmd & DEVMGR_SOFT_CMD_BM) != 0u) ? 1 : 0;
            fRtlPref0 = devmgr_soft_bar_mem_ok(&g_aPci[i],
                                               DEVMGR_SOFT_RTL_PREF_BAR0);
            fRtlPref2 = devmgr_soft_bar_mem_ok(&g_aPci[i],
                                               DEVMGR_SOFT_RTL_PREF_BAR2);
            if (fRtlPref0 != 0) {
                u64RtlPa0 = g_aPci[i].aBar[DEVMGR_SOFT_RTL_PREF_BAR0].u64Pa;
                u64RtlCb0 = g_aPci[i].aBar[DEVMGR_SOFT_RTL_PREF_BAR0].u64Cb;
            }
            if (fRtlPref2 != 0) {
                u64RtlPa2 = g_aPci[i].aBar[DEVMGR_SOFT_RTL_PREF_BAR2].u64Pa;
                u64RtlCb2 = g_aPci[i].aBar[DEVMGR_SOFT_RTL_PREF_BAR2].u64Cb;
            }
            if (iRtlBar >= 0) {
                u64RtlPa = g_aPci[i].aBar[iRtlBar].u64Pa;
                u64RtlCb = g_aPci[i].aBar[iRtlBar].u64Cb;
                u8RtlIs64 = g_aPci[i].aBar[iRtlBar].u8Is64;
                u8RtlPref = g_aPci[i].aBar[iRtlBar].u8Prefetch;
            }
        }
        if (!fXhci && g_aPci[i].u16Vendor == DEVMGR_VID_INTEL &&
            g_aPci[i].u16Device == DEVMGR_DID_XHCI) {
            fXhci = 1;
            iXhci = (int)i;
            fXhciClassOk = devmgr_soft_class_ok_xhci(&g_aPci[i]);
            fXhciProgIfOk = devmgr_soft_progif_ok_xhci(&g_aPci[i]);
            iXhciBar = devmgr_soft_fn_first_mem_bar(&g_aPci[i]);
            u16XhciCmd = g_aPci[i].u16Cmd;
            fXhciIo = ((u16XhciCmd & DEVMGR_SOFT_CMD_IO) != 0u) ? 1 : 0;
            fXhciMem = ((u16XhciCmd & DEVMGR_SOFT_CMD_MEM) != 0u) ? 1 : 0;
            fXhciBm = ((u16XhciCmd & DEVMGR_SOFT_CMD_BM) != 0u) ? 1 : 0;
            fXhciPref0 = devmgr_soft_bar_mem_ok(&g_aPci[i],
                                                DEVMGR_SOFT_XHCI_PREF_BAR0);
            if (fXhciPref0 != 0) {
                u64XhciPa0 = g_aPci[i].aBar[DEVMGR_SOFT_XHCI_PREF_BAR0].u64Pa;
                u64XhciCb0 = g_aPci[i].aBar[DEVMGR_SOFT_XHCI_PREF_BAR0].u64Cb;
            }
            if (iXhciBar >= 0) {
                u64XhciPa = g_aPci[i].aBar[iXhciBar].u64Pa;
                u64XhciCb = g_aPci[i].aBar[iXhciBar].u64Cb;
                u8XhciIs64 = g_aPci[i].aBar[iXhciBar].u8Is64;
                u8XhciPref = g_aPci[i].aBar[iXhciBar].u8Prefetch;
            }
        }
    }

    /* Soft residual self-check: dual-DoD hit bits match HIT/MISS lamps. */
    if (fRtl) {
        u32HitsExpect |= GJ_DEVMGR_SOFT_MATCH_RTL8168;
    }
    if (fXhci) {
        u32HitsExpect |= GJ_DEVMGR_SOFT_MATCH_XHCI_A12F;
    }
    fHitsOk = (g_u32DualDodHits == u32HitsExpect) ? 1 : 0;

    /*
     * pref_ok: product-host preferred BAR residual present when HIT.
     * MISS is honesty-ok (QEMU often MISS dual-DoD IDs). Soft!=product.
     */
    fPrefOk = 1;
    if (fRtl && (fRtlPref0 == 0 || fRtlPref2 == 0)) {
        fPrefOk = 0;
    }
    if (fXhci && fXhciPref0 == 0) {
        fPrefOk = 0;
    }

    /*
     * Grep: devmgr: soft residual lean
     * Lean dual-DoD HIT/MISS + BAR geom + pref BAR + cmd bits + DDI/UDX path.
     * Product hosts = rtl8168_udx (DoD B) + xhci_udx (DoD A); mint=OPEN.
     * Match graph product remains OPEN. Soft inventory seed only.
     */
    kprintf("devmgr: soft residual lean dual_dod "
            "rtl8168=%s idx=%d bar=%d pa=0x%lx cb=0x%lx is64=%u pref=%u "
            "class_ok=%d "
            "pref_rtl0=%d pa0=0x%lx cb0=0x%lx "
            "pref_rtl2=%d pa2=0x%lx cb2=0x%lx "
            "cmd=0x%04x io=%d mem=%d bm=%d "
            "xhci_a12f=%s idx=%d bar=%d pa=0x%lx cb=0x%lx is64=%u pref=%u "
            "class_ok=%d progif_ok=%d "
            "pref_xhci0=%d pa0=0x%lx cb0=0x%lx "
            "cmd=0x%04x io=%d mem=%d bm=%d "
            "pref_ok=%d "
            "pci_n=%u bdf_n=%u win_n=%u win_seed=%u hits=0x%x hits_ok=%d "
            "path=inventory->find->window_note->ddi_door->"
            "UDX->mint_OPEN "
            "hosts=rtl8168_udx+xhci_udx "
            "dod_a=OPEN dod_b=OPEN match_graph=OPEN "
            "soft=1 product=0 mint=OPEN Soft!=product G-AC-1 "
            "dual=MIT_OR_Apache-2.0 "
            "(DDI/UDX seed; not sole BAR owner; freestanding SKIP residual; "
            "no .ko product; no live VT-d; not TX/RX; not BOT/MSC; "
            "pref_bar!=product MAP; cmd_bits!=pci_enable)\n",
            fRtl ? "HIT" : "MISS", iRtl, iRtlBar,
            (unsigned long)u64RtlPa, (unsigned long)u64RtlCb,
            (unsigned)u8RtlIs64, (unsigned)u8RtlPref, fRtlClassOk,
            fRtlPref0, (unsigned long)u64RtlPa0, (unsigned long)u64RtlCb0,
            fRtlPref2, (unsigned long)u64RtlPa2, (unsigned long)u64RtlCb2,
            (unsigned)u16RtlCmd, fRtlIo, fRtlMem, fRtlBm,
            fXhci ? "HIT" : "MISS", iXhci, iXhciBar,
            (unsigned long)u64XhciPa, (unsigned long)u64XhciCb,
            (unsigned)u8XhciIs64, (unsigned)u8XhciPref,
            fXhciClassOk, fXhciProgIfOk,
            fXhciPref0, (unsigned long)u64XhciPa0, (unsigned long)u64XhciCb0,
            (unsigned)u16XhciCmd, fXhciIo, fXhciMem, fXhciBm,
            fPrefOk,
            (unsigned)g_cPciUsed, (unsigned)g_cBdfUsed,
            (unsigned)g_cWinNotes, (unsigned)g_cDualDodWinSeed,
            (unsigned)g_u32DualDodHits, fHitsOk);
}

/**
 * Soft PCI scan residual deepen once-lamp (DDI dual-DoD seed honesty; W4).
 *
 * After first CF8 fill: per-ID soft idx + BDF + class/progif + IRQ +
 * cmd/status + subsystem + MF + first-MEM BAR (pa/cb/is64/pref) +
 * product-host preferred BAR residual (rtl0+rtl2, xhci0) + pref_ok +
 * cmd_io/mem/bm residual bits + mem/io counts + class_ok (+ xHCI progif_ok)
 * + soft window seed count.
 * Match graph OPEN. Product hosts = UDX. Soft!=product / G-AC-1.
 * No stamp storms. greppable: devmgr: soft pci scan residual deepen
 */
static void
devmgr_soft_pci_scan_residual_deepen_once(void)
{
    u32 i;
    const struct gj_devmgr_pci_fn *pRtl = NULL;
    const struct gj_devmgr_pci_fn *pXhci = NULL;
    int iRtl = -1;
    int iXhci = -1;
    int iRtlBar = -1;
    int iXhciBar = -1;
    int fRtlClassOk = 0;
    int fXhciClassOk = 0;
    int fXhciProgIfOk = 0;
    int fRtlPref0 = 0;
    int fRtlPref2 = 0;
    int fXhciPref0 = 0;
    int fPrefOk = 0;
    int fRtlIo = 0;
    int fRtlMem = 0;
    int fRtlBm = 0;
    int fXhciIo = 0;
    int fXhciMem = 0;
    int fXhciBm = 0;
    u8  u8RtlIs64 = 0u;
    u8  u8RtlPref = 0u;
    u8  u8XhciIs64 = 0u;
    u8  u8XhciPref = 0u;
    u64 u64RtlPa = 0;
    u64 u64RtlCb = 0;
    u64 u64RtlPa0 = 0;
    u64 u64RtlCb0 = 0;
    u64 u64RtlPa2 = 0;
    u64 u64RtlCb2 = 0;
    u64 u64XhciPa = 0;
    u64 u64XhciCb = 0;
    u64 u64XhciPa0 = 0;
    u64 u64XhciCb0 = 0;

    if (g_fScanDeepenOnce != 0u) {
        return;
    }
    g_fScanDeepenOnce = 1u;

    for (i = 0u; i < g_cPciUsed; i++) {
        if (pRtl == NULL && g_aPci[i].u16Vendor == DEVMGR_VID_REALTEK &&
            g_aPci[i].u16Device == DEVMGR_DID_RTL8168) {
            pRtl = &g_aPci[i];
            iRtl = (int)i;
            iRtlBar = devmgr_soft_fn_first_mem_bar(pRtl);
            fRtlClassOk = devmgr_soft_class_ok_rtl(pRtl);
            fRtlIo = ((pRtl->u16Cmd & DEVMGR_SOFT_CMD_IO) != 0u) ? 1 : 0;
            fRtlMem = ((pRtl->u16Cmd & DEVMGR_SOFT_CMD_MEM) != 0u) ? 1 : 0;
            fRtlBm = ((pRtl->u16Cmd & DEVMGR_SOFT_CMD_BM) != 0u) ? 1 : 0;
            fRtlPref0 = devmgr_soft_bar_mem_ok(pRtl, DEVMGR_SOFT_RTL_PREF_BAR0);
            fRtlPref2 = devmgr_soft_bar_mem_ok(pRtl, DEVMGR_SOFT_RTL_PREF_BAR2);
            if (fRtlPref0 != 0) {
                u64RtlPa0 = pRtl->aBar[DEVMGR_SOFT_RTL_PREF_BAR0].u64Pa;
                u64RtlCb0 = pRtl->aBar[DEVMGR_SOFT_RTL_PREF_BAR0].u64Cb;
            }
            if (fRtlPref2 != 0) {
                u64RtlPa2 = pRtl->aBar[DEVMGR_SOFT_RTL_PREF_BAR2].u64Pa;
                u64RtlCb2 = pRtl->aBar[DEVMGR_SOFT_RTL_PREF_BAR2].u64Cb;
            }
            if (iRtlBar >= 0) {
                u64RtlPa = pRtl->aBar[iRtlBar].u64Pa;
                u64RtlCb = pRtl->aBar[iRtlBar].u64Cb;
                u8RtlIs64 = pRtl->aBar[iRtlBar].u8Is64;
                u8RtlPref = pRtl->aBar[iRtlBar].u8Prefetch;
            }
        }
        if (pXhci == NULL && g_aPci[i].u16Vendor == DEVMGR_VID_INTEL &&
            g_aPci[i].u16Device == DEVMGR_DID_XHCI) {
            pXhci = &g_aPci[i];
            iXhci = (int)i;
            iXhciBar = devmgr_soft_fn_first_mem_bar(pXhci);
            fXhciClassOk = devmgr_soft_class_ok_xhci(pXhci);
            fXhciProgIfOk = devmgr_soft_progif_ok_xhci(pXhci);
            fXhciIo = ((pXhci->u16Cmd & DEVMGR_SOFT_CMD_IO) != 0u) ? 1 : 0;
            fXhciMem = ((pXhci->u16Cmd & DEVMGR_SOFT_CMD_MEM) != 0u) ? 1 : 0;
            fXhciBm = ((pXhci->u16Cmd & DEVMGR_SOFT_CMD_BM) != 0u) ? 1 : 0;
            fXhciPref0 = devmgr_soft_bar_mem_ok(pXhci,
                                                DEVMGR_SOFT_XHCI_PREF_BAR0);
            if (fXhciPref0 != 0) {
                u64XhciPa0 = pXhci->aBar[DEVMGR_SOFT_XHCI_PREF_BAR0].u64Pa;
                u64XhciCb0 = pXhci->aBar[DEVMGR_SOFT_XHCI_PREF_BAR0].u64Cb;
            }
            if (iXhciBar >= 0) {
                u64XhciPa = pXhci->aBar[iXhciBar].u64Pa;
                u64XhciCb = pXhci->aBar[iXhciBar].u64Cb;
                u8XhciIs64 = pXhci->aBar[iXhciBar].u8Is64;
                u8XhciPref = pXhci->aBar[iXhciBar].u8Prefetch;
            }
        }
    }

    /* pref_ok: product-host preferred BAR residual present when HIT. */
    fPrefOk = 1;
    if (pRtl != NULL && (fRtlPref0 == 0 || fRtlPref2 == 0)) {
        fPrefOk = 0;
    }
    if (pXhci != NULL && fXhciPref0 == 0) {
        fPrefOk = 0;
    }

    /*
     * Grep: devmgr: soft pci scan residual deepen
     * Real CF8 residual deepen for DDI dual-DoD seeds (1 line).
     * Soft inventory look only - match_graph=OPEN; product hosts=UDX.
     * idx feeds ddi_door OPEN/GET; pref BAR + is64/pref seed MAP_BAR honesty.
     * cmd_io/mem/bm soft observe only (not pci_enable product).
     */
    kprintf("devmgr: soft pci scan residual deepen "
            "rtl8168=%s idx=%d bdf=%02x:%02x.%u class=%02x:%02x:%02x "
            "rev=0x%02x irq=%u pin=%u cmd=0x%04x st=0x%04x cap=0x%02x "
            "subsys=%04x:%04x mf=%u bar=%d pa=0x%lx cb=0x%lx "
            "is64=%u pref=%u mem_n=%u io_n=%u class_ok=%d "
            "pref_rtl0=%d pa0=0x%lx cb0=0x%lx "
            "pref_rtl2=%d pa2=0x%lx cb2=0x%lx "
            "io=%d mem=%d bm=%d "
            "xhci_a12f=%s idx=%d bdf=%02x:%02x.%u class=%02x:%02x:%02x "
            "rev=0x%02x irq=%u pin=%u cmd=0x%04x st=0x%04x cap=0x%02x "
            "subsys=%04x:%04x mf=%u bar=%d pa=0x%lx cb=0x%lx "
            "is64=%u pref=%u mem_n=%u io_n=%u class_ok=%d progif_ok=%d "
            "pref_xhci0=%d pa0=0x%lx cb0=0x%lx "
            "io=%d mem=%d bm=%d "
            "pref_ok=%d "
            "pci_n=%u hits=0x%x win_seed=%u "
            "match_graph=OPEN hosts=UDX mint=OPEN "
            "dod_a=OPEN dod_b=OPEN "
            "soft=1 product=0 Soft!=product G-AC-1 dual=MIT_OR_Apache-2.0 "
            "(DDI seed; not TX/RX; not BOT/MSC; no sole BAR owner; "
            "no live VT-d; freestanding SKIP residual; "
            "pref_bar!=product MAP; cmd_bits!=pci_enable)\n",
            pRtl != NULL ? "HIT" : "MISS",
            iRtl,
            pRtl != NULL ? (unsigned)pRtl->bdf.u8Bus : 0u,
            pRtl != NULL ? (unsigned)pRtl->bdf.u8Slot : 0u,
            pRtl != NULL ? (unsigned)pRtl->bdf.u8Func : 0u,
            pRtl != NULL ? (unsigned)pRtl->u8Class : 0u,
            pRtl != NULL ? (unsigned)pRtl->u8Subclass : 0u,
            pRtl != NULL ? (unsigned)pRtl->u8ProgIf : 0u,
            pRtl != NULL ? (unsigned)pRtl->u8RevId : 0u,
            pRtl != NULL ? (unsigned)pRtl->u8IntLine : 0u,
            pRtl != NULL ? (unsigned)pRtl->u8IntPin : 0u,
            pRtl != NULL ? (unsigned)pRtl->u16Cmd : 0u,
            pRtl != NULL ? (unsigned)pRtl->u16Status : 0u,
            pRtl != NULL ? (unsigned)pRtl->u8CapPtr : 0u,
            pRtl != NULL ? (unsigned)pRtl->u16SubsysVend : 0u,
            pRtl != NULL ? (unsigned)pRtl->u16SubsysId : 0u,
            pRtl != NULL ? (unsigned)pRtl->u8MultiFunc : 0u,
            iRtlBar, (unsigned long)u64RtlPa, (unsigned long)u64RtlCb,
            (unsigned)u8RtlIs64, (unsigned)u8RtlPref,
            pRtl != NULL ? (unsigned)pRtl->u8BarMemN : 0u,
            pRtl != NULL ? (unsigned)pRtl->u8BarIoN : 0u,
            fRtlClassOk,
            fRtlPref0, (unsigned long)u64RtlPa0, (unsigned long)u64RtlCb0,
            fRtlPref2, (unsigned long)u64RtlPa2, (unsigned long)u64RtlCb2,
            fRtlIo, fRtlMem, fRtlBm,
            pXhci != NULL ? "HIT" : "MISS",
            iXhci,
            pXhci != NULL ? (unsigned)pXhci->bdf.u8Bus : 0u,
            pXhci != NULL ? (unsigned)pXhci->bdf.u8Slot : 0u,
            pXhci != NULL ? (unsigned)pXhci->bdf.u8Func : 0u,
            pXhci != NULL ? (unsigned)pXhci->u8Class : 0u,
            pXhci != NULL ? (unsigned)pXhci->u8Subclass : 0u,
            pXhci != NULL ? (unsigned)pXhci->u8ProgIf : 0u,
            pXhci != NULL ? (unsigned)pXhci->u8RevId : 0u,
            pXhci != NULL ? (unsigned)pXhci->u8IntLine : 0u,
            pXhci != NULL ? (unsigned)pXhci->u8IntPin : 0u,
            pXhci != NULL ? (unsigned)pXhci->u16Cmd : 0u,
            pXhci != NULL ? (unsigned)pXhci->u16Status : 0u,
            pXhci != NULL ? (unsigned)pXhci->u8CapPtr : 0u,
            pXhci != NULL ? (unsigned)pXhci->u16SubsysVend : 0u,
            pXhci != NULL ? (unsigned)pXhci->u16SubsysId : 0u,
            pXhci != NULL ? (unsigned)pXhci->u8MultiFunc : 0u,
            iXhciBar, (unsigned long)u64XhciPa, (unsigned long)u64XhciCb,
            (unsigned)u8XhciIs64, (unsigned)u8XhciPref,
            pXhci != NULL ? (unsigned)pXhci->u8BarMemN : 0u,
            pXhci != NULL ? (unsigned)pXhci->u8BarIoN : 0u,
            fXhciClassOk, fXhciProgIfOk,
            fXhciPref0, (unsigned long)u64XhciPa0, (unsigned long)u64XhciCb0,
            fXhciIo, fXhciMem, fXhciBm,
            fPrefOk,
            (unsigned)g_cPciUsed, (unsigned)g_u32DualDodHits,
            (unsigned)g_cDualDodWinSeed);
}

/* ---- Inline I/O + config helpers (this file only) --------------------- */

static inline void
devmgr_outl(u16 u16Port, u32 u32Val)
{
    __asm__ volatile("outl %0, %1" : : "a"(u32Val), "Nd"(u16Port));
}

static inline u32
devmgr_inl(u16 u16Port)
{
    u32 u32Val;

    __asm__ volatile("inl %1, %0" : "=a"(u32Val) : "Nd"(u16Port));
    return u32Val;
}

static u32
devmgr_pci_cfg_addr(u8 u8Bus, u8 u8Slot, u8 u8Func, u8 u8Off)
{
    return 0x80000000u | ((u32)u8Bus << 16) | ((u32)u8Slot << 11) |
           ((u32)u8Func << 8) | ((u32)u8Off & 0xfcu);
}

static u32
devmgr_pci_cfg_read32(u8 u8Bus, u8 u8Slot, u8 u8Func, u8 u8Off)
{
    devmgr_outl(DEVMGR_PCI_CFG_ADDR,
                devmgr_pci_cfg_addr(u8Bus, u8Slot, u8Func, u8Off));
    return devmgr_inl(DEVMGR_PCI_CFG_DATA);
}

static void
devmgr_pci_cfg_write32(u8 u8Bus, u8 u8Slot, u8 u8Func, u8 u8Off, u32 u32Val)
{
    devmgr_outl(DEVMGR_PCI_CFG_ADDR,
                devmgr_pci_cfg_addr(u8Bus, u8Slot, u8Func, u8Off));
    devmgr_outl(DEVMGR_PCI_CFG_DATA, u32Val);
}

/**
 * Classic BAR size probe: write all-1s, read mask, restore original.
 * Handles I/O, 32-bit MEM, and 64-bit MEM (consumes next BAR index).
 * On 64-bit, *pSkipNext is set so the caller skips the high dword slot.
 */
static void
devmgr_bar_probe(u8 u8Bus, u8 u8Slot, u8 u8Func, u8 u8BarIdx,
                 struct gj_devmgr_bar *pBar, int *pSkipNext)
{
    u8  u8Off;
    u32 u32Lo;
    u32 u32SaveLo;
    u32 u32RawLo;
    u64 u64Pa;
    u64 u64Sz;
    int fMem;
    int fPref;
    int f64;

    if (pSkipNext != NULL) {
        *pSkipNext = 0;
    }
    if (pBar == NULL || u8BarIdx >= GJ_DEVMGR_SOFT_BAR_MAX) {
        return;
    }

    memset(pBar, 0, sizeof(*pBar));
    pBar->u8Index = u8BarIdx;

    u8Off = (u8)(0x10u + (u8)(u8BarIdx * 4u));
    u32Lo = devmgr_pci_cfg_read32(u8Bus, u8Slot, u8Func, u8Off);
    u32SaveLo = u32Lo;

    /* Empty / unimplemented BAR: leave zeros. */
    if (u32Lo == 0u || u32Lo == 0xffffffffu) {
        return;
    }

    fMem = ((u32Lo & 1u) == 0u) ? 1 : 0;
    fPref = 0;
    f64 = 0;
    u64Pa = 0;
    u64Sz = 0;

    if (!fMem) {
        /* I/O BAR: low 2 bits are flags; base in bits 31:2. */
        u64Pa = (u64)(u32Lo & ~0x3u);

        devmgr_pci_cfg_write32(u8Bus, u8Slot, u8Func, u8Off, 0xffffffffu);
        u32RawLo = devmgr_pci_cfg_read32(u8Bus, u8Slot, u8Func, u8Off);
        devmgr_pci_cfg_write32(u8Bus, u8Slot, u8Func, u8Off, u32SaveLo);

        u32RawLo &= ~0x3u;
        if (u32RawLo != 0u) {
            u64Sz = (u64)((~u32RawLo) + 1u);
            /* I/O space is 32-bit; clamp absurd masks. */
            if (u64Sz > 0x10000ull) {
                u64Sz = 0x10000ull;
            }
        }
    } else {
        u32 u32Type = (u32Lo >> 1) & 3u;

        fPref = ((u32Lo & 0x8u) != 0u) ? 1 : 0;
        u64Pa = (u64)(u32Lo & ~0xfu);

        if (u32Type == 2u && u8BarIdx < 5u) {
            /* 64-bit memory BAR: high dword is next BAR register. */
            u8  u8OffHi = (u8)(u8Off + 4u);
            u32 u32Hi = devmgr_pci_cfg_read32(u8Bus, u8Slot, u8Func, u8OffHi);
            u32 u32SaveHi = u32Hi;
            u32 u32RawHi;
            u64 u64Mask;

            f64 = 1;
            u64Pa |= ((u64)u32Hi << 32);

            devmgr_pci_cfg_write32(u8Bus, u8Slot, u8Func, u8Off, 0xffffffffu);
            devmgr_pci_cfg_write32(u8Bus, u8Slot, u8Func, u8OffHi,
                                   0xffffffffu);
            u32RawLo = devmgr_pci_cfg_read32(u8Bus, u8Slot, u8Func, u8Off);
            u32RawHi = devmgr_pci_cfg_read32(u8Bus, u8Slot, u8Func, u8OffHi);
            devmgr_pci_cfg_write32(u8Bus, u8Slot, u8Func, u8Off, u32SaveLo);
            devmgr_pci_cfg_write32(u8Bus, u8Slot, u8Func, u8OffHi, u32SaveHi);

            u64Mask = ((u64)u32RawHi << 32) | (u64)(u32RawLo & ~0xfu);
            if (u64Mask != 0ull) {
                u64Sz = (~u64Mask) + 1ull;
            }
            if (pSkipNext != NULL) {
                *pSkipNext = 1;
            }
        } else {
            /* 32-bit memory BAR (type 0) or reserved type 1. */
            devmgr_pci_cfg_write32(u8Bus, u8Slot, u8Func, u8Off, 0xffffffffu);
            u32RawLo = devmgr_pci_cfg_read32(u8Bus, u8Slot, u8Func, u8Off);
            devmgr_pci_cfg_write32(u8Bus, u8Slot, u8Func, u8Off, u32SaveLo);

            u32RawLo &= ~0xfu;
            if (u32RawLo != 0u) {
                u64Sz = (u64)((~u32RawLo) + 1u);
            }
        }
    }

    pBar->u8Mem = (u8)fMem;
    pBar->u8Prefetch = (u8)fPref;
    pBar->u8Is64 = (u8)f64;
    pBar->u64Pa = u64Pa;
    pBar->u64Cb = u64Sz;
}

static void
devmgr_pci_record_fn(u8 u8Bus, u8 u8Slot, u8 u8Func, u16 u16Vend, u16 u16Dev)
{
    struct gj_devmgr_pci_fn *pFn;
    u32 u32ClassDw;
    u32 u32HdrDw;
    u32 u32CmdSt;
    u32 u32Subsys;
    u32 u32CapDw;
    u32 u32IntDw;
    u8  u8Hdr;
    u8  iBar;
    u8  cMem;
    u8  cIo;
    int fSkipNext;

    if (g_cPciUsed >= GJ_DEVMGR_SOFT_BDF_MAX) {
        return;
    }

    pFn = &g_aPci[g_cPciUsed];
    memset(pFn, 0, sizeof(*pFn));
    pFn->bdf.u8Bus = u8Bus;
    pFn->bdf.u8Slot = u8Slot;
    pFn->bdf.u8Func = u8Func;
    pFn->u16Vendor = u16Vend;
    pFn->u16Device = u16Dev;

    /* Class code dword @ 0x08: rev | prog-if | subclass | class */
    u32ClassDw = devmgr_pci_cfg_read32(u8Bus, u8Slot, u8Func, 0x08u);
    pFn->u8RevId = (u8)(u32ClassDw & 0xffu);
    pFn->u8ProgIf = (u8)((u32ClassDw >> 8) & 0xffu);
    pFn->u8Subclass = (u8)((u32ClassDw >> 16) & 0xffu);
    pFn->u8Class = (u8)((u32ClassDw >> 24) & 0xffu);

    /* Command/status dword @ 0x04. */
    u32CmdSt = devmgr_pci_cfg_read32(u8Bus, u8Slot, u8Func, 0x04u);
    pFn->u16Cmd = (u16)(u32CmdSt & 0xffffu);
    pFn->u16Status = (u16)((u32CmdSt >> 16) & 0xffffu);

    /* Header Type @ 0x0e (in dword @ 0x0c). Bit7 = multi-function. */
    u32HdrDw = devmgr_pci_cfg_read32(u8Bus, u8Slot, u8Func, 0x0cu);
    u8Hdr = (u8)((u32HdrDw >> 16) & 0xffu);
    pFn->u8HdrType = (u8)(u8Hdr & 0x7fu);
    pFn->u8MultiFunc = ((u8Hdr & 0x80u) != 0u) ? 1u : 0u;

    /* Subsystem vendor/device @ 0x2c (type-0 only; soft 0 on type-1). */
    if (pFn->u8HdrType == 0u) {
        u32Subsys = devmgr_pci_cfg_read32(u8Bus, u8Slot, u8Func, 0x2cu);
        pFn->u16SubsysVend = (u16)(u32Subsys & 0xffffu);
        pFn->u16SubsysId = (u16)((u32Subsys >> 16) & 0xffffu);

        /* Cap list pointer @ 0x34 (valid when status bit4 CapList). */
        u32CapDw = devmgr_pci_cfg_read32(u8Bus, u8Slot, u8Func, 0x34u);
        if ((pFn->u16Status & 0x10u) != 0u) {
            pFn->u8CapPtr = (u8)(u32CapDw & 0xfcu);
        } else {
            pFn->u8CapPtr = 0u;
        }

        /* Interrupt line/pin @ 0x3c. */
        u32IntDw = devmgr_pci_cfg_read32(u8Bus, u8Slot, u8Func, 0x3cu);
        pFn->u8IntLine = (u8)(u32IntDw & 0xffu);
        pFn->u8IntPin = (u8)((u32IntDw >> 8) & 0xffu);
    }

    /*
     * BAR0..5 size probe only for type-0 headers. Type-1 bridge layout
     * reuses 0x18+ for bus numbers - never write all-1s there.
     */
    cMem = 0u;
    cIo = 0u;
    if (pFn->u8HdrType == 0u) {
        fSkipNext = 0;
        for (iBar = 0u; iBar < GJ_DEVMGR_SOFT_BAR_MAX; iBar++) {
            if (fSkipNext) {
                /* High dword of prior 64-bit BAR - leave zero stub. */
                pFn->aBar[iBar].u8Index = iBar;
                fSkipNext = 0;
                continue;
            }
            devmgr_bar_probe(u8Bus, u8Slot, u8Func, iBar, &pFn->aBar[iBar],
                             &fSkipNext);
            if (pFn->aBar[iBar].u64Cb != 0ull) {
                if (pFn->aBar[iBar].u8Mem != 0u) {
                    cMem++;
                } else {
                    cIo++;
                }
            }
        }
    }
    pFn->u8BarMemN = cMem;
    pFn->u8BarIoN = cIo;

    /* Soft dual-DoD match residual bits (inventory look; graph OPEN). */
    if (u16Vend == DEVMGR_VID_REALTEK && u16Dev == DEVMGR_DID_RTL8168) {
        g_u32DualDodHits |= GJ_DEVMGR_SOFT_MATCH_RTL8168;
    } else if (u16Vend == DEVMGR_VID_INTEL && u16Dev == DEVMGR_DID_XHCI) {
        g_u32DualDodHits |= GJ_DEVMGR_SOFT_MATCH_XHCI_A12F;
    }

    g_cPciUsed++;
}

/**
 * Full type-0 mechanism walk: bus 0..7 x slot 0..31, MF-aware funcs.
 * Func0 always visited when present. Funcs 1..7 only when MF bit set on
 * the function that declared multi-function (classic residual deepen).
 * Skip empty vendor 0xffff / 0x0000.
 */
static void
devmgr_pci_walk(void)
{
    u32 u32Bus;
    u8  u8Slot;
    u8  u8Func;

    g_cPciUsed = 0u;
    g_u32DualDodHits = 0u;
    g_cDualDodWinSeed = 0u;
    memset(g_aPci, 0, sizeof(g_aPci));

    for (u32Bus = 0u; u32Bus < DEVMGR_PCI_BUS_MAX; u32Bus++) {
        for (u8Slot = 0u; u8Slot < DEVMGR_PCI_SLOT_MAX; u8Slot++) {
            u8  u8Bus = (u8)u32Bus;
            u32 u32Id;
            u16 u16Vend;
            u16 u16Dev;
            int fMulti = 0;

            /* Func 0 first: presence + multi-function residual. */
            u32Id = devmgr_pci_cfg_read32(u8Bus, u8Slot, 0u, 0x00u);
            u16Vend = (u16)(u32Id & 0xffffu);
            u16Dev = (u16)((u32Id >> 16) & 0xffffu);

            if (u16Vend == 0xffffu || u16Vend == 0x0000u) {
                continue;
            }

            devmgr_pci_record_fn(u8Bus, u8Slot, 0u, u16Vend, u16Dev);
            if (g_cPciUsed >= GJ_DEVMGR_SOFT_BDF_MAX) {
                return;
            }
            /* MF residual: last recorded slot func0 carries u8MultiFunc. */
            if (g_cPciUsed > 0u) {
                fMulti = (g_aPci[g_cPciUsed - 1u].u8MultiFunc != 0u) ? 1 : 0;
            }

            if (fMulti == 0) {
                continue;
            }

            for (u8Func = 1u; u8Func < DEVMGR_PCI_FUNC_MAX; u8Func++) {
                u32Id = devmgr_pci_cfg_read32(u8Bus, u8Slot, u8Func, 0x00u);
                u16Vend = (u16)(u32Id & 0xffffu);
                u16Dev = (u16)((u32Id >> 16) & 0xffffu);

                if (u16Vend == 0xffffu || u16Vend == 0x0000u) {
                    continue;
                }

                devmgr_pci_record_fn(u8Bus, u8Slot, u8Func, u16Vend, u16Dev);

                if (g_cPciUsed >= GJ_DEVMGR_SOFT_BDF_MAX) {
                    return;
                }
            }
        }
    }
}

static void
devmgr_pci_emit_found_lamps(void)
{
    u32 i;
    int fRtl = 0;
    int fXhci = 0;
    const struct gj_devmgr_pci_fn *pRtl = NULL;
    const struct gj_devmgr_pci_fn *pXhci = NULL;

    /* Once-lamp only - NO stamp storms on repeated pci_scan. */
    if (g_fFoundLampOnce != 0u) {
        return;
    }
    g_fFoundLampOnce = 1u;

    for (i = 0u; i < g_cPciUsed; i++) {
        if (g_aPci[i].u16Vendor == DEVMGR_VID_REALTEK &&
            g_aPci[i].u16Device == DEVMGR_DID_RTL8168) {
            fRtl = 1;
            pRtl = &g_aPci[i];
        }
        if (g_aPci[i].u16Vendor == DEVMGR_VID_INTEL &&
            g_aPci[i].u16Device == DEVMGR_DID_XHCI) {
            fXhci = 1;
            pXhci = &g_aPci[i];
        }
    }

    /*
     * Grep: devmgr: soft found 10ec:8168
     * Once HIT or MISS so dual-DoD B soft seed is greppable
     * on QEMU (MISS) and G752VT (HIT). Soft!=product - not TX/RX;
     * product NIC path = rtl8168_udx over DDI/UDX (mint OPEN).
     */
    if (fRtl && pRtl != NULL) {
        int iBar = devmgr_soft_fn_first_mem_bar(pRtl);
        int fPref0 = devmgr_soft_bar_mem_ok(pRtl, DEVMGR_SOFT_RTL_PREF_BAR0);
        int fPref2 = devmgr_soft_bar_mem_ok(pRtl, DEVMGR_SOFT_RTL_PREF_BAR2);
        int fIo = ((pRtl->u16Cmd & DEVMGR_SOFT_CMD_IO) != 0u) ? 1 : 0;
        int fMem = ((pRtl->u16Cmd & DEVMGR_SOFT_CMD_MEM) != 0u) ? 1 : 0;
        int fBm = ((pRtl->u16Cmd & DEVMGR_SOFT_CMD_BM) != 0u) ? 1 : 0;

        kprintf("devmgr: soft found 10ec:8168 HIT @ %02x:%02x.%u "
                "class=%02x:%02x:%02x irq=%u pin=%u cmd=0x%04x "
                "bar=%d pa=0x%lx cb=0x%lx mem_n=%u class_ok=%d "
                "pref_rtl0=%d pref_rtl2=%d io=%d mem=%d bm=%d "
                "soft=1 product=0 Soft!=product G-AC-1 "
                "dod_b=seed host=rtl8168_udx match_graph=OPEN "
                "(not TX/RX; not freestanding product; mint=OPEN; "
                "pref_bar!=product MAP)\n",
                (unsigned)pRtl->bdf.u8Bus, (unsigned)pRtl->bdf.u8Slot,
                (unsigned)pRtl->bdf.u8Func,
                (unsigned)pRtl->u8Class, (unsigned)pRtl->u8Subclass,
                (unsigned)pRtl->u8ProgIf,
                (unsigned)pRtl->u8IntLine, (unsigned)pRtl->u8IntPin,
                (unsigned)pRtl->u16Cmd,
                iBar,
                (unsigned long)(iBar >= 0 ? pRtl->aBar[iBar].u64Pa : 0ull),
                (unsigned long)(iBar >= 0 ? pRtl->aBar[iBar].u64Cb : 0ull),
                (unsigned)pRtl->u8BarMemN,
                devmgr_soft_class_ok_rtl(pRtl),
                fPref0, fPref2, fIo, fMem, fBm);
    } else {
        kprintf("devmgr: soft found 10ec:8168 MISS "
                "soft=1 product=0 Soft!=product G-AC-1 "
                "dod_b=seed host=rtl8168_udx match_graph=OPEN "
                "(inventory look only; not sole BAR owner)\n");
    }
    /*
     * Grep: devmgr: soft found 8086:a12f
     * Once HIT or MISS. Dual-DoD A soft seed - not product BOT/MSC.
     * Product USB path = xhci_udx over DDI/UDX (mint OPEN).
     */
    if (fXhci && pXhci != NULL) {
        int iBar = devmgr_soft_fn_first_mem_bar(pXhci);
        int fPref0 = devmgr_soft_bar_mem_ok(pXhci, DEVMGR_SOFT_XHCI_PREF_BAR0);
        int fIo = ((pXhci->u16Cmd & DEVMGR_SOFT_CMD_IO) != 0u) ? 1 : 0;
        int fMem = ((pXhci->u16Cmd & DEVMGR_SOFT_CMD_MEM) != 0u) ? 1 : 0;
        int fBm = ((pXhci->u16Cmd & DEVMGR_SOFT_CMD_BM) != 0u) ? 1 : 0;

        kprintf("devmgr: soft found 8086:a12f HIT @ %02x:%02x.%u "
                "class=%02x:%02x:%02x irq=%u pin=%u cmd=0x%04x "
                "bar=%d pa=0x%lx cb=0x%lx mem_n=%u class_ok=%d "
                "pref_xhci0=%d io=%d mem=%d bm=%d "
                "soft=1 product=0 Soft!=product G-AC-1 "
                "dod_a=seed host=xhci_udx match_graph=OPEN "
                "(not BOT/MSC; not freestanding product; mint=OPEN; "
                "pref_bar!=product MAP)\n",
                (unsigned)pXhci->bdf.u8Bus, (unsigned)pXhci->bdf.u8Slot,
                (unsigned)pXhci->bdf.u8Func,
                (unsigned)pXhci->u8Class, (unsigned)pXhci->u8Subclass,
                (unsigned)pXhci->u8ProgIf,
                (unsigned)pXhci->u8IntLine, (unsigned)pXhci->u8IntPin,
                (unsigned)pXhci->u16Cmd,
                iBar,
                (unsigned long)(iBar >= 0 ? pXhci->aBar[iBar].u64Pa : 0ull),
                (unsigned long)(iBar >= 0 ? pXhci->aBar[iBar].u64Cb : 0ull),
                (unsigned)pXhci->u8BarMemN,
                devmgr_soft_class_ok_xhci(pXhci),
                fPref0, fIo, fMem, fBm);
    } else {
        kprintf("devmgr: soft found 8086:a12f MISS "
                "soft=1 product=0 Soft!=product G-AC-1 "
                "dod_a=seed host=xhci_udx match_graph=OPEN "
                "(inventory look only)\n");
    }
}

void
devmgr_soft_init(void)
{
    g_cInitCalls++;
    if (g_fReady) {
        /* Idempotent: silent re-entry (NO stamp storms). */
        return;
    }

    g_cWinUsed = 0u;
    g_cWinNotes = 0u;
    g_cWinRejects = 0u;
    g_cPciScanCalls = 0u;
    g_cBdfUsed = 0u;
    g_cBdfRegs = 0u;
    g_cBdfRejects = 0u;
    g_cPciUsed = 0u;
    g_u32DualDodHits = 0u;
    g_cDualDodWinSeed = 0u;
    g_fScanned = 0;
    g_fInitLampOnce = 0u;
    g_fScanLampOnce = 0u;
    g_fFoundLampOnce = 0u;
    g_fWinNoteLampOnce = 0u;
    g_fWinRejectLampOnce = 0u;
    g_fResidualOnce = 0u;
    g_fScanDeepenOnce = 0u;
    memset(g_aWin, 0, sizeof(g_aWin));
    memset(g_aBdf, 0, sizeof(g_aBdf));
    memset(g_aBdfOk, 0, sizeof(g_aBdfOk));
    memset(g_aPci, 0, sizeof(g_aPci));
    g_fReady = 1;

    /*
     * Soft lamps for ABI-first DDI/UDX seed (docs/DDI_SOFT.md).
     * First init defers CF8/CFC walk - call devmgr_soft_pci_scan() to fill.
     * Soft!=product - no cap mint, no VT-d program. Once-lamp only.
     * Product hosts = UDX; match graph = OPEN.
     */
    if (g_fInitLampOnce == 0u) {
        g_fInitLampOnce = 1u;
        /* Grep: devmgr: soft init PASS */
        kprintf("devmgr: soft init PASS win_max=%u bar_max=%u bdf_max=%u "
                "soft=1 product=0 Soft!=product G-AC-1 "
                "path=DDI/UDX hosts=UDX match_graph=OPEN mint=OPEN "
                "dual=MIT_OR_Apache-2.0\n",
                (unsigned)GJ_DEVMGR_SOFT_WIN_MAX,
                (unsigned)GJ_DEVMGR_SOFT_BAR_MAX,
                (unsigned)GJ_DEVMGR_SOFT_BDF_MAX);
        /* Scan deferred until first pci_scan (greppable honesty). */
        kprintf("devmgr: soft pci scan deferred "
                "(call pci_scan to fill; Soft!=product; UDX seed; "
                "match_graph=OPEN)\n");
        /* Grep: devmgr: soft dma window note */
        kprintf("devmgr: soft dma window note "
                "(soft table ready; no VT-d; Soft!=product; mint=OPEN)\n");
    }
}

int
devmgr_soft_ready(void)
{
    return g_fReady;
}

u32
devmgr_soft_pci_scan(struct gj_devmgr_bdf *pOut, u32 cMax)
{
    int fFirstFill = 0;

    g_cPciScanCalls++;

    if (!g_fReady) {
        devmgr_soft_init();
    }

    /* First call walks config space; later calls reuse soft inventory. */
    if (!g_fScanned) {
        devmgr_pci_walk();
        g_fScanned = 1;
        fFirstFill = 1;
        /* Soft dual-DoD first-MEM window seed residual (DDI DMA_NOTE path). */
        devmgr_soft_dual_dod_window_seed();
    }

    if (pOut != NULL && cMax > 0u) {
        u32 i;
        u32 cCopy = g_cPciUsed;

        if (cCopy > cMax) {
            cCopy = cMax;
        }
        for (i = 0u; i < cCopy; i++) {
            pOut[i] = g_aPci[i].bdf;
        }
    }

    /*
     * Once-lamp scan PASS + found HIT|MISS + lean residual + deepen
     * (NO stamp storms). Subsequent scans return inventory silently
     * (counters still advance).
     */
    if (fFirstFill != 0 || g_fScanLampOnce == 0u) {
        if (g_fScanLampOnce == 0u) {
            g_fScanLampOnce = 1u;
            /* Grep: devmgr: soft pci scan PASS n=N */
            kprintf("devmgr: soft pci scan PASS n=%u call=%u hits=0x%x "
                    "soft=1 product=0 Soft!=product G-AC-1 "
                    "(inventory seed for ddi_door/UDX; not sole BAR owner; "
                    "match_graph=OPEN hosts=UDX mint=OPEN)\n",
                    (unsigned)g_cPciUsed, (unsigned)g_cPciScanCalls,
                    (unsigned)g_u32DualDodHits);
        }
        devmgr_pci_emit_found_lamps();
        devmgr_soft_residual_lean_once();
        devmgr_soft_pci_scan_residual_deepen_once();
    }

    return g_cPciUsed;
}

int
devmgr_soft_bdf_register(const struct gj_devmgr_bdf *pBdf)
{
    u32 i;

    if (!g_fReady) {
        devmgr_soft_init();
    }

    if (pBdf == NULL) {
        g_cBdfRejects++;
        return -1;
    }

    /* Soft slot/func range (PCI type-0). */
    if (pBdf->u8Slot > 31u || pBdf->u8Func > 7u) {
        g_cBdfRejects++;
        return -1;
    }

    /* Dedup: return existing soft index if already registered. */
    for (i = 0u; i < g_cBdfUsed; i++) {
        if (g_aBdfOk[i] != 0u &&
            g_aBdf[i].u8Bus == pBdf->u8Bus &&
            g_aBdf[i].u8Slot == pBdf->u8Slot &&
            g_aBdf[i].u8Func == pBdf->u8Func) {
            return (int)i;
        }
    }

    if (g_cBdfUsed >= GJ_DEVMGR_SOFT_BDF_MAX) {
        g_cBdfRejects++;
        return -1;
    }

    i = g_cBdfUsed;
    g_aBdf[i] = *pBdf;
    g_aBdf[i].u8Pad = 0u;
    g_aBdfOk[i] = 1u;
    g_cBdfUsed++;
    g_cBdfRegs++;

    return (int)i;
}

int
devmgr_soft_find_by_id(u16 u16Vend, u16 u16Dev, struct gj_devmgr_pci_fn *pOut)
{
    u32 i;

    if (pOut == NULL) {
        return -1;
    }

    if (!g_fReady) {
        devmgr_soft_init();
    }

    /* Ensure inventory exists so find works after init alone. */
    if (!g_fScanned) {
        (void)devmgr_soft_pci_scan(NULL, 0u);
    }

    for (i = 0u; i < g_cPciUsed; i++) {
        if (g_aPci[i].u16Vendor == u16Vend &&
            g_aPci[i].u16Device == u16Dev) {
            *pOut = g_aPci[i];
            return 0;
        }
    }

    memset(pOut, 0, sizeof(*pOut));
    return -1;
}

int
devmgr_soft_index_by_id(u16 u16Vend, u16 u16Dev)
{
    u32 i;

    if (!g_fReady) {
        devmgr_soft_init();
    }

    if (!g_fScanned) {
        (void)devmgr_soft_pci_scan(NULL, 0u);
    }

    for (i = 0u; i < g_cPciUsed; i++) {
        if (g_aPci[i].u16Vendor == u16Vend &&
            g_aPci[i].u16Device == u16Dev) {
            return (int)i;
        }
    }
    return -1;
}

u32
devmgr_soft_dual_dod_hits(void)
{
    if (!g_fReady) {
        return 0u;
    }
    if (!g_fScanned) {
        (void)devmgr_soft_pci_scan(NULL, 0u);
    }
    return g_u32DualDodHits;
}

u32
devmgr_soft_count(void)
{
    return g_cPciUsed;
}

int
devmgr_soft_get(u32 iIdx, struct gj_devmgr_pci_fn *pOut)
{
    if (pOut == NULL || iIdx >= g_cPciUsed) {
        return -1;
    }
    *pOut = g_aPci[iIdx];
    return 0;
}

int
devmgr_soft_dma_window_note(const struct gj_devmgr_bdf *pBdf,
                            u64 u64PaBase, u64 u64Cb, u8 u8Rw,
                            struct gj_devmgr_window_grant *pGrantOut)
{
    struct gj_devmgr_window_grant *pSlot;
    u32 iWin;

    if (!g_fReady) {
        devmgr_soft_init();
    }

    if (pBdf == NULL || u64Cb == 0u) {
        g_cWinRejects++;
        if (pGrantOut != NULL) {
            memset(pGrantOut, 0, sizeof(*pGrantOut));
            pGrantOut->u32WinId = 0xffffffffu;
        }
        return -1;
    }

    /* Soft slot/func range (PCI type-0). */
    if (pBdf->u8Slot > 31u || pBdf->u8Func > 7u) {
        g_cWinRejects++;
        if (pGrantOut != NULL) {
            memset(pGrantOut, 0, sizeof(*pGrantOut));
            pGrantOut->bdf = *pBdf;
            pGrantOut->u64PaBase = u64PaBase;
            pGrantOut->u64Cb = u64Cb;
            pGrantOut->u8Rw = u8Rw;
            pGrantOut->u32WinId = 0xffffffffu;
        }
        return -1;
    }

    /* Overflow check on PA range. */
    if (u64PaBase + u64Cb < u64PaBase) {
        g_cWinRejects++;
        if (pGrantOut != NULL) {
            memset(pGrantOut, 0, sizeof(*pGrantOut));
            pGrantOut->bdf = *pBdf;
            pGrantOut->u64PaBase = u64PaBase;
            pGrantOut->u64Cb = u64Cb;
            pGrantOut->u8Rw = u8Rw;
            pGrantOut->u32WinId = 0xffffffffu;
        }
        return -1;
    }

    if (g_cWinUsed >= GJ_DEVMGR_SOFT_WIN_MAX) {
        g_cWinRejects++;
        if (pGrantOut != NULL) {
            memset(pGrantOut, 0, sizeof(*pGrantOut));
            pGrantOut->bdf = *pBdf;
            pGrantOut->u64PaBase = u64PaBase;
            pGrantOut->u64Cb = u64Cb;
            pGrantOut->u8Rw = u8Rw;
            pGrantOut->u32WinId = 0xffffffffu;
        }
        /* Once-lamp reject (NO stamp storms). */
        if (g_fWinRejectLampOnce == 0u) {
            g_fWinRejectLampOnce = 1u;
            /* Grep: devmgr: soft dma window note */
            kprintf("devmgr: soft dma window note REJECT full bdf=%u:%u.%u "
                    "pa=0x%lx cb=0x%lx soft=1 Soft!=product\n",
                    (unsigned)pBdf->u8Bus, (unsigned)pBdf->u8Slot,
                    (unsigned)pBdf->u8Func, (unsigned long)u64PaBase,
                    (unsigned long)u64Cb);
        }
        return -1;
    }

    iWin = g_cWinUsed;
    pSlot = &g_aWin[iWin];
    pSlot->bdf = *pBdf;
    pSlot->u64PaBase = u64PaBase;
    pSlot->u64Cb = u64Cb;
    pSlot->u8Rw = u8Rw;
    pSlot->u8Ok = 1u;
    pSlot->u8Pad[0] = 0u;
    pSlot->u8Pad[1] = 0u;
    pSlot->u32WinId = iWin;
    g_cWinUsed++;
    g_cWinNotes++;

    if (pGrantOut != NULL) {
        *pGrantOut = *pSlot;
    }

    /* Once-lamp accept (NO stamp storms). Counters always advance. */
    if (g_fWinNoteLampOnce == 0u) {
        g_fWinNoteLampOnce = 1u;
        /* Grep: devmgr: soft dma window note */
        kprintf("devmgr: soft dma window note id=%u bdf=%u:%u.%u pa=0x%lx "
                "cb=0x%lx rw=0x%x used=%u soft=1 product=0 Soft!=product "
                "mint=OPEN G-AC-1 dual=MIT_OR_Apache-2.0 "
                "(UDX DMA window seed; no live VT-d; match_graph=OPEN)\n",
                (unsigned)iWin, (unsigned)pBdf->u8Bus,
                (unsigned)pBdf->u8Slot, (unsigned)pBdf->u8Func,
                (unsigned long)u64PaBase, (unsigned long)u64Cb,
                (unsigned)u8Rw, (unsigned)g_cWinUsed);
        /* Residual may fire after first real window note as well. */
        devmgr_soft_residual_lean_once();
    }

    return 0;
}

int
devmgr_soft_note_window_grant(const struct gj_devmgr_bdf *pBdf,
                              u64 u64PaBase, u64 u64Cb, u8 u8Rw,
                              struct gj_devmgr_window_grant *pGrantOut)
{
    /* Soft table only - no live VT-d program (docs/DDI_SOFT.md). */
    return devmgr_soft_dma_window_note(pBdf, u64PaBase, u64Cb, u8Rw,
                                       pGrantOut);
}

u32
devmgr_soft_pci_scan_calls(void)
{
    return g_cPciScanCalls;
}

u32
devmgr_soft_bdf_count(void)
{
    return g_cBdfUsed;
}

u32
devmgr_soft_bdf_rejects(void)
{
    return g_cBdfRejects;
}

u32
devmgr_soft_window_notes(void)
{
    return g_cWinNotes;
}

u32
devmgr_soft_window_rejects(void)
{
    return g_cWinRejects;
}
