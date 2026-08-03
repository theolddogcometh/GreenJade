/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Host MMIO window table + inject helpers (UDX_HOST_LIBC).
 * Window table is also available freestanding for granted maps.
 *
 * Soft inventory (Wave 126 exclusive deepen; this unit only) —
 * greppable "udx: host soft …":
 *   udx: host soft inventory …
 *   udx: host soft window …
 *   udx: host soft lookup …
 *   udx: host soft bar …
 *   udx: host soft irq …
 *   udx: host soft path …
 *   udx: host soft wave …
 * Soft DDI host surface (docs/DDI_SOFT.md shape; local types; no kernel hdr):
 *   udx: soft ddi host note PASS   via udx_host_soft_init() (first path)
 *   udx: soft ddi-ready …          via udx_host_soft_note_ddi()
 *   udx: soft ddi lifecycle note   via udx_host_soft_lifecycle_note()
 *   udx: soft ddi dma not-granted  via udx_host_soft_dma_window_request()
 *   udx: soft ddi bind PASS|SKIP   via udx_host_bind_scan / bind_by_id
 * Pure observation; never gates host skeleton PASS or freestanding path.
 * Soft ≠ product grant / multi-server confine. No GPL.
 * Soft ≠ skeleton PASS. greppable: udx: host soft
 * greppable: udx: soft ddi host note PASS
 * greppable: udx: soft ddi-ready
 * greppable: udx: soft ddi bind PASS
 * greppable: udx: soft ddi bind SKIP
 *
 * Note: PCI inject/remove + install_granted live in pci.c (host.h).
 * This unit owns windows + BAR poke + soft DDI host types + freestanding
 * GJ_SYS_DDI bind (open/map BAR0 → window → ioremap granted PA).
 */
#include "udx_internal.h"

#include <udx/ddi.h>
#include <udx/host.h>
#include <udx/irq.h>
#include <udx/udx.h>

#include <stdarg.h>

#if defined(UDX_HOST_LIBC)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif

#define UDX_HOST_WIN_MAX 64
/* Soft wave stamp for greppable inventory lines. */
#define UDX_HOST_SOFT_WAVE 70u

struct udx_host_window {
    u64   u64Phys;
    u64   u64Len;
    void *pVa;
    u8    u8Used;
};

static struct udx_host_window g_aWin[UDX_HOST_WIN_MAX];

/*
 * Soft host window / BAR inventory (Wave 126 exclusive deepen).
 * Cumulative for this process. greppable: udx: host soft …
 * Never hard-gates; wrap OK if ever hit.
 */
static u32 g_u32HostWinReg;       /* register enter */
static u32 g_u32HostWinRegOk;     /* new slot filled */
static u32 g_u32HostWinRegUpd;    /* same phys base update */
static u32 g_u32HostWinRegFull;   /* table full reject */
static u32 g_u32HostWinRegOvf;    /* phys+len overflow reject */
static u32 g_u32HostWinRegInval;  /* null va / zero len */
static u32 g_u32HostWinUnreg;     /* unregister enter */
static u32 g_u32HostWinUnregHit;  /* unregister found slot */
static u32 g_u32HostWinUnregMiss; /* unregister no match */
static u32 g_u32HostWinLookup;    /* lookup enter */
static u32 g_u32HostWinLookHit;   /* lookup hit */
static u32 g_u32HostWinLookMiss;  /* lookup miss */
static u32 g_u32HostWinLookInval; /* zero len / overflow */
static u32 g_u32HostWinLive;      /* currently used slots */
static u32 g_u32HostWinPeak;      /* peak used slots */
static u32 g_u32HostFireIrq;      /* udx_host_fire_irq */
static u32 g_u32HostBarWrite;     /* BAR write* ok */
static u32 g_u32HostBarRead;      /* BAR read* ok */
static u32 g_u32HostBarFail;      /* BAR access fail (inval/nodev) */
static u32 g_u32HostBarNosys;     /* freestanding NOSYS path */
static u32 g_u32HostBarMemcpy;    /* memcpy_to/from ok */
static u32 g_u32HostBarW;         /* write width tallies: l/w/b/q */
static u32 g_u32HostBarR;         /* read width tallies: l/w/b/q (combined) */
static u32 g_u32HostLogN;         /* soft inventory dumps */
static u8  g_fHostSoftOnce;       /* one-shot after first window ok */

/* Soft DDI host lamps (ABI-first; never product grants). */
static u8  g_fDdiHostInited;      /* soft ready after first udx_host_soft_init */
static u32 g_u32DdiInitCalls;     /* udx_host_soft_init entries */
static u32 g_u32DdiDmaReq;        /* dma window request enters */
static u32 g_u32DdiDmaReject;     /* soft not-granted / inval rejects */
static u32 g_u32DdiLifecycleN;    /* lifecycle note dumps */
static u32 g_u32DdiBdfSnap;       /* bdf_from_pdev ok */
static u32 g_u32DdiBarSnap;       /* bar_snapshot ok */
static u32 g_u32DdiBindEnter;     /* bind_scan / bind_by_id enter */
static u32 g_u32DdiBindPass;      /* soft ddi bind PASS */
static u32 g_u32DdiBindSkip;      /* soft ddi bind SKIP */
static u32 g_u32DdiBindSys;       /* GJ_SYS_DDI attempts */
static u32 g_u32DdiBindSysFail;   /* syscall fail / ENOSYS */

static void host_soft_inc(u32 *pu32);
static void host_soft_note_peak(u32 *pu32Peak, u32 u32Val);
static void host_soft_emit(const char *szFmt, ...);
static void host_soft_inventory_log(void);
static void host_soft_maybe_once(void);
static u32  host_soft_live_windows(void);

static void
host_soft_inc(u32 *pu32)
{
    if (pu32 != NULL && *pu32 < 0xffffffffu) {
        (*pu32)++;
    }
}

/** Soft: raise peak if u32Val is higher (diagnostics only). */
static void
host_soft_note_peak(u32 *pu32Peak, u32 u32Val)
{
    if (pu32Peak != NULL && u32Val > *pu32Peak) {
        *pu32Peak = u32Val;
    }
}

static u32
host_soft_live_windows(void)
{
    u32 iWin;
    u32 u32Live;

    u32Live = 0;
    for (iWin = 0; iWin < UDX_HOST_WIN_MAX; iWin++) {
        if (g_aWin[iWin].u8Used) {
            if (u32Live < 0xffffffffu) {
                u32Live++;
            }
        }
    }
    return u32Live;
}

/*
 * Soft inventory emit path — host console; does not use udx_printk so
 * core soft printk tallies stay product-path pure.
 */
static void
host_soft_emit(const char *szFmt, ...)
{
    if (szFmt == NULL) {
        return;
    }
#if defined(UDX_HOST_LIBC)
    {
        va_list apArgs;

        va_start(apArgs, szFmt);
        (void)vprintf(szFmt, apArgs);
        va_end(apArgs);
    }
#else
    (void)szFmt;
#endif
}

/**
 * Greppable soft host inventory (Wave 126 exclusive deepen).
 * Prefix-stable "udx: host soft …" — never hard-gates; observation only.
 *
 * greppable: udx: host soft
 */
static void
host_soft_inventory_log(void)
{
    u32 u32Host;
    u32 u32Live;

    host_soft_inc(&g_u32HostLogN);
    u32Live = host_soft_live_windows();
    g_u32HostWinLive = u32Live;
    host_soft_note_peak(&g_u32HostWinPeak, u32Live);

#if defined(UDX_HOST_LIBC)
    u32Host = 1u;
#else
    u32Host = 0u;
#endif

    /* Grep: udx: host soft inventory */
    host_soft_emit("udx: host soft inventory win_reg_ok=%u win_reg_upd=%u "
                   "win_full=%u win_ovf=%u unreg_hit=%u lookup_hit=%u "
                   "lookup_miss=%u fire_irq=%u bar_w=%u bar_r=%u "
                   "log_n=%u wave=%u\n",
                   g_u32HostWinRegOk, g_u32HostWinRegUpd, g_u32HostWinRegFull,
                   g_u32HostWinRegOvf, g_u32HostWinUnregHit,
                   g_u32HostWinLookHit, g_u32HostWinLookMiss,
                   g_u32HostFireIrq, g_u32HostBarWrite, g_u32HostBarRead,
                   g_u32HostLogN, UDX_HOST_SOFT_WAVE);

    /* Grep: udx: host soft window */
    host_soft_emit("udx: host soft window max=%u live=%u peak=%u "
                   "reg_enter=%u reg_ok=%u reg_upd=%u reg_full=%u "
                   "reg_ovf=%u reg_inval=%u unreg=%u unreg_hit=%u "
                   "unreg_miss=%u\n",
                   (u32)UDX_HOST_WIN_MAX, u32Live, g_u32HostWinPeak,
                   g_u32HostWinReg, g_u32HostWinRegOk, g_u32HostWinRegUpd,
                   g_u32HostWinRegFull, g_u32HostWinRegOvf,
                   g_u32HostWinRegInval, g_u32HostWinUnreg,
                   g_u32HostWinUnregHit, g_u32HostWinUnregMiss);

    /* Grep: udx: host soft lookup */
    host_soft_emit("udx: host soft lookup enter=%u hit=%u miss=%u inval=%u "
                   "range_cover=1 sub_window=1\n",
                   g_u32HostWinLookup, g_u32HostWinLookHit,
                   g_u32HostWinLookMiss, g_u32HostWinLookInval);

    /* Grep: udx: host soft bar */
    host_soft_emit("udx: host soft bar write_ok=%u read_ok=%u fail=%u "
                   "nosys=%u memcpy_ok=%u width_w=%u width_r=%u "
                   "host_libc=%u freestanding_nosys=%u\n",
                   g_u32HostBarWrite, g_u32HostBarRead, g_u32HostBarFail,
                   g_u32HostBarNosys, g_u32HostBarMemcpy, g_u32HostBarW,
                   g_u32HostBarR, u32Host, (u32Host == 0u) ? 1u : 0u);

    /* Grep: udx: host soft irq */
    host_soft_emit("udx: host soft irq fire=%u equiv=udx_irq_dispatch "
                   "badge_claim=immediate soft_mask=irq_table\n",
                   g_u32HostFireIrq);

    /*
     * Path catalog — what this soft surface is / is not.
     * greppable: udx: host soft path
     */
    host_soft_emit("udx: host soft path "
                   "reg=udx_host_window_register "
                   "unreg=udx_host_window_unregister "
                   "lookup=udx_host_window_lookup "
                   "fire=udx_host_fire_irq "
                   "bar_rw=udx_host_bar_* "
                   "pci_inject=pci.c "
                   "ddi_bind=udx_host_bind_by_id "
                   "sys_ddi=103 "
                   "skeleton_gate=0 hard_gate=0 soft=1\n");

    /* Grep: udx: host soft wave */
    host_soft_emit("udx: host soft wave n=%u unit=host exclusive=1 "
                   "prefix=udx:_host_soft deepen=1 "
                   "(soft inventory; never gates skeleton PASS)\n",
                   UDX_HOST_SOFT_WAVE);

    /*
     * Grep: udx: host soft honesty (Wave 126 exclusive deepen).
     * Soft inventory ≠ product multi-server confine.
     */
    host_soft_emit("udx: host soft honesty multi_server=0 confine=0 bar3=0 "
                  "exclusive=1 soft=1 product_kernel=OPEN wave=%u\n",
                  UDX_HOST_SOFT_WAVE);

    /* ABI-first soft DDI readiness + first host init PASS (observation). */
    udx_host_soft_init();
    udx_host_soft_note_ddi();
    udx_host_soft_lifecycle_note();

    /* Soft DDI bind inventory tallies (PASS/SKIP are separate lamps). */
    host_soft_emit("udx: host soft ddi_bind enter=%u pass=%u skip=%u "
                   "sys=%u sys_fail=%u "
                   "targets=10ec:8168,8086:a12f "
                   "soft=1 product=0\n",
                   g_u32DdiBindEnter, g_u32DdiBindPass, g_u32DdiBindSkip,
                   g_u32DdiBindSys, g_u32DdiBindSysFail);
}

/** Soft: one-shot inventory after first successful window register. */
static void
host_soft_maybe_once(void)
{
    if (g_fHostSoftOnce != 0) {
        return;
    }
    if (g_u32HostWinRegOk == 0 && g_u32HostWinRegUpd == 0 &&
        g_u32HostFireIrq == 0) {
        return;
    }
    g_fHostSoftOnce = 1;
    /* First host activity path: soft DDI host note PASS (idempotent). */
    udx_host_soft_init();
    host_soft_inventory_log();
}

static int
host_range_ok(u64 u64Base, u64 u64Len, u64 u64Off, u64 u64Width)
{
    if (u64Base == 0 || u64Len == 0 || u64Width == 0) {
        return 0;
    }
    if (u64Off > u64Len) {
        return 0;
    }
    if (u64Width > u64Len - u64Off) {
        return 0;
    }
    return 1;
}

void
udx_host_window_register(u64 u64Phys, void *pVa, u64 u64Len)
{
    u32 iWin;
    u32 iFree;

    host_soft_inc(&g_u32HostWinReg);
    if (pVa == NULL || u64Len == 0) {
        host_soft_inc(&g_u32HostWinRegInval);
        return;
    }
    /* Overflow: phys+len must not wrap. */
    if (u64Phys + u64Len < u64Phys) {
        host_soft_inc(&g_u32HostWinRegOvf);
        udx_printk("udx: host window reject overflow phys=%llx len=%llx\n",
                   (unsigned long long)u64Phys,
                   (unsigned long long)u64Len);
        return;
    }

    /* Update existing mapping for the same phys base. */
    iFree = UDX_HOST_WIN_MAX;
    for (iWin = 0; iWin < UDX_HOST_WIN_MAX; iWin++) {
        if (g_aWin[iWin].u8Used && g_aWin[iWin].u64Phys == u64Phys) {
            g_aWin[iWin].u64Len = u64Len;
            g_aWin[iWin].pVa = pVa;
            host_soft_inc(&g_u32HostWinRegUpd);
            host_soft_maybe_once();
            return;
        }
        if (!g_aWin[iWin].u8Used && iFree == UDX_HOST_WIN_MAX) {
            iFree = iWin;
        }
    }
    if (iFree >= UDX_HOST_WIN_MAX) {
        host_soft_inc(&g_u32HostWinRegFull);
        udx_printk("udx: host window table full\n");
        return;
    }
    g_aWin[iFree].u64Phys = u64Phys;
    g_aWin[iFree].u64Len = u64Len;
    g_aWin[iFree].pVa = pVa;
    g_aWin[iFree].u8Used = 1;
    host_soft_inc(&g_u32HostWinRegOk);
    g_u32HostWinLive = host_soft_live_windows();
    host_soft_note_peak(&g_u32HostWinPeak, g_u32HostWinLive);
    host_soft_maybe_once();
}

void
udx_host_window_unregister(u64 u64Phys)
{
    u32 iWin;

    host_soft_inc(&g_u32HostWinUnreg);
    for (iWin = 0; iWin < UDX_HOST_WIN_MAX; iWin++) {
        if (g_aWin[iWin].u8Used && g_aWin[iWin].u64Phys == u64Phys) {
            g_aWin[iWin].u8Used = 0;
            g_aWin[iWin].pVa = NULL;
            g_aWin[iWin].u64Len = 0;
            g_aWin[iWin].u64Phys = 0;
            host_soft_inc(&g_u32HostWinUnregHit);
            g_u32HostWinLive = host_soft_live_windows();
            return;
        }
    }
    host_soft_inc(&g_u32HostWinUnregMiss);
}

void *
udx_host_window_lookup(u64 u64Phys, u64 u64Len, u64 *pu64OffOut)
{
    u32 iWin;

    host_soft_inc(&g_u32HostWinLookup);
    if (u64Len == 0) {
        host_soft_inc(&g_u32HostWinLookInval);
        return NULL;
    }
    if (u64Phys + u64Len < u64Phys) {
        host_soft_inc(&g_u32HostWinLookInval);
        return NULL; /* overflow */
    }

    for (iWin = 0; iWin < UDX_HOST_WIN_MAX; iWin++) {
        u64 u64Base;
        u64 u64End;
        u64 u64ReqEnd;

        if (!g_aWin[iWin].u8Used) {
            continue;
        }
        u64Base = g_aWin[iWin].u64Phys;
        u64End = u64Base + g_aWin[iWin].u64Len;
        if (u64End < u64Base) {
            continue; /* corrupt / overflow entry */
        }
        u64ReqEnd = u64Phys + u64Len;
        if (u64Phys >= u64Base && u64ReqEnd <= u64End) {
            if (pu64OffOut) {
                *pu64OffOut = u64Phys - u64Base;
            }
            host_soft_inc(&g_u32HostWinLookHit);
            return (u8 *)g_aWin[iWin].pVa + (u64Phys - u64Base);
        }
    }
    host_soft_inc(&g_u32HostWinLookMiss);
    return NULL;
}

void
udx_host_fire_irq(int nIrq)
{
    host_soft_inc(&g_u32HostFireIrq);
    host_soft_maybe_once();
    udx_irq_dispatch(nIrq);
}

#if defined(UDX_HOST_LIBC)
static udx_status_t
host_bar_va(struct udx_pci_dev *pPdev, int nBar, u64 u64Off, u64 u64Width,
            void **ppVa)
{
    u64 u64Phys;
    u64 u64Len;
    void *pVa;

    if (pPdev == NULL || ppVa == NULL || nBar < 0 || nBar > 5) {
        return UDX_ERR_INVAL;
    }
    u64Phys = pPdev->aBarPhys[nBar];
    u64Len = pPdev->aBarLen[nBar];
    if (!host_range_ok(u64Phys, u64Len, u64Off, u64Width)) {
        return UDX_ERR_INVAL;
    }
    /* Prefer whole-BAR window; fall back to offset-sized sub-window. */
    pVa = udx_host_window_lookup(u64Phys, u64Len, NULL);
    if (pVa == NULL) {
        pVa = udx_host_window_lookup(u64Phys + u64Off, u64Width, NULL);
        if (pVa == NULL) {
            return UDX_ERR_NODEV;
        }
        *ppVa = pVa;
        return UDX_OK;
    }
    *ppVa = (u8 *)pVa + (size_t)u64Off;
    return UDX_OK;
}
#endif /* UDX_HOST_LIBC */

udx_status_t
udx_host_bar_writel(struct udx_pci_dev *pPdev, int nBar, u64 u64Off, u32 u32Val)
{
#if !defined(UDX_HOST_LIBC)
    (void)pPdev;
    (void)nBar;
    (void)u64Off;
    (void)u32Val;
    host_soft_inc(&g_u32HostBarNosys);
    return UDX_ERR_NOSYS;
#else
    void *pVa;
    udx_status_t st;

    st = host_bar_va(pPdev, nBar, u64Off, 4, &pVa);
    if (st != UDX_OK) {
        host_soft_inc(&g_u32HostBarFail);
        return st;
    }
    *(volatile u32 *)pVa = u32Val;
    host_soft_inc(&g_u32HostBarWrite);
    host_soft_inc(&g_u32HostBarW);
    return UDX_OK;
#endif
}

udx_status_t
udx_host_bar_readl(struct udx_pci_dev *pPdev, int nBar, u64 u64Off, u32 *pu32Val)
{
#if !defined(UDX_HOST_LIBC)
    (void)pPdev;
    (void)nBar;
    (void)u64Off;
    (void)pu32Val;
    host_soft_inc(&g_u32HostBarNosys);
    return UDX_ERR_NOSYS;
#else
    void *pVa;
    udx_status_t st;

    if (pu32Val == NULL) {
        host_soft_inc(&g_u32HostBarFail);
        return UDX_ERR_INVAL;
    }
    st = host_bar_va(pPdev, nBar, u64Off, 4, &pVa);
    if (st != UDX_OK) {
        host_soft_inc(&g_u32HostBarFail);
        return st;
    }
    *pu32Val = *(volatile u32 *)pVa;
    host_soft_inc(&g_u32HostBarRead);
    host_soft_inc(&g_u32HostBarR);
    return UDX_OK;
#endif
}

udx_status_t
udx_host_bar_writew(struct udx_pci_dev *pPdev, int nBar, u64 u64Off, u16 u16Val)
{
#if !defined(UDX_HOST_LIBC)
    (void)pPdev;
    (void)nBar;
    (void)u64Off;
    (void)u16Val;
    host_soft_inc(&g_u32HostBarNosys);
    return UDX_ERR_NOSYS;
#else
    void *pVa;
    udx_status_t st;

    st = host_bar_va(pPdev, nBar, u64Off, 2, &pVa);
    if (st != UDX_OK) {
        host_soft_inc(&g_u32HostBarFail);
        return st;
    }
    *(volatile u16 *)pVa = u16Val;
    host_soft_inc(&g_u32HostBarWrite);
    host_soft_inc(&g_u32HostBarW);
    return UDX_OK;
#endif
}

udx_status_t
udx_host_bar_readw(struct udx_pci_dev *pPdev, int nBar, u64 u64Off, u16 *pu16Val)
{
#if !defined(UDX_HOST_LIBC)
    (void)pPdev;
    (void)nBar;
    (void)u64Off;
    (void)pu16Val;
    host_soft_inc(&g_u32HostBarNosys);
    return UDX_ERR_NOSYS;
#else
    void *pVa;
    udx_status_t st;

    if (pu16Val == NULL) {
        host_soft_inc(&g_u32HostBarFail);
        return UDX_ERR_INVAL;
    }
    st = host_bar_va(pPdev, nBar, u64Off, 2, &pVa);
    if (st != UDX_OK) {
        host_soft_inc(&g_u32HostBarFail);
        return st;
    }
    *pu16Val = *(volatile u16 *)pVa;
    host_soft_inc(&g_u32HostBarRead);
    host_soft_inc(&g_u32HostBarR);
    return UDX_OK;
#endif
}

udx_status_t
udx_host_bar_writeb(struct udx_pci_dev *pPdev, int nBar, u64 u64Off, u8 u8Val)
{
#if !defined(UDX_HOST_LIBC)
    (void)pPdev;
    (void)nBar;
    (void)u64Off;
    (void)u8Val;
    host_soft_inc(&g_u32HostBarNosys);
    return UDX_ERR_NOSYS;
#else
    void *pVa;
    udx_status_t st;

    st = host_bar_va(pPdev, nBar, u64Off, 1, &pVa);
    if (st != UDX_OK) {
        host_soft_inc(&g_u32HostBarFail);
        return st;
    }
    *(volatile u8 *)pVa = u8Val;
    host_soft_inc(&g_u32HostBarWrite);
    host_soft_inc(&g_u32HostBarW);
    return UDX_OK;
#endif
}

udx_status_t
udx_host_bar_readb(struct udx_pci_dev *pPdev, int nBar, u64 u64Off, u8 *pu8Val)
{
#if !defined(UDX_HOST_LIBC)
    (void)pPdev;
    (void)nBar;
    (void)u64Off;
    (void)pu8Val;
    host_soft_inc(&g_u32HostBarNosys);
    return UDX_ERR_NOSYS;
#else
    void *pVa;
    udx_status_t st;

    if (pu8Val == NULL) {
        host_soft_inc(&g_u32HostBarFail);
        return UDX_ERR_INVAL;
    }
    st = host_bar_va(pPdev, nBar, u64Off, 1, &pVa);
    if (st != UDX_OK) {
        host_soft_inc(&g_u32HostBarFail);
        return st;
    }
    *pu8Val = *(volatile u8 *)pVa;
    host_soft_inc(&g_u32HostBarRead);
    host_soft_inc(&g_u32HostBarR);
    return UDX_OK;
#endif
}

udx_status_t
udx_host_bar_writeq(struct udx_pci_dev *pPdev, int nBar, u64 u64Off, u64 u64Val)
{
#if !defined(UDX_HOST_LIBC)
    (void)pPdev;
    (void)nBar;
    (void)u64Off;
    (void)u64Val;
    host_soft_inc(&g_u32HostBarNosys);
    return UDX_ERR_NOSYS;
#else
    void *pVa;
    udx_status_t st;

    st = host_bar_va(pPdev, nBar, u64Off, 8, &pVa);
    if (st != UDX_OK) {
        host_soft_inc(&g_u32HostBarFail);
        return st;
    }
    *(volatile u64 *)pVa = u64Val;
    host_soft_inc(&g_u32HostBarWrite);
    host_soft_inc(&g_u32HostBarW);
    return UDX_OK;
#endif
}

udx_status_t
udx_host_bar_readq(struct udx_pci_dev *pPdev, int nBar, u64 u64Off, u64 *pu64Val)
{
#if !defined(UDX_HOST_LIBC)
    (void)pPdev;
    (void)nBar;
    (void)u64Off;
    (void)pu64Val;
    host_soft_inc(&g_u32HostBarNosys);
    return UDX_ERR_NOSYS;
#else
    void *pVa;
    udx_status_t st;

    if (pu64Val == NULL) {
        host_soft_inc(&g_u32HostBarFail);
        return UDX_ERR_INVAL;
    }
    st = host_bar_va(pPdev, nBar, u64Off, 8, &pVa);
    if (st != UDX_OK) {
        host_soft_inc(&g_u32HostBarFail);
        return st;
    }
    *pu64Val = *(volatile u64 *)pVa;
    host_soft_inc(&g_u32HostBarRead);
    host_soft_inc(&g_u32HostBarR);
    return UDX_OK;
#endif
}

udx_status_t
udx_host_bar_memcpy_to(struct udx_pci_dev *pPdev, int nBar, u64 u64Off,
                       const void *pSrc, size_t cb)
{
#if !defined(UDX_HOST_LIBC)
    (void)pPdev;
    (void)nBar;
    (void)u64Off;
    (void)pSrc;
    (void)cb;
    host_soft_inc(&g_u32HostBarNosys);
    return UDX_ERR_NOSYS;
#else
    void *pVa;
    udx_status_t st;
    size_t iByte;
    const u8 *pIn;
    volatile u8 *pOut;

    if (pSrc == NULL || cb == 0) {
        host_soft_inc(&g_u32HostBarFail);
        return UDX_ERR_INVAL;
    }
    st = host_bar_va(pPdev, nBar, u64Off, (u64)cb, &pVa);
    if (st != UDX_OK) {
        host_soft_inc(&g_u32HostBarFail);
        return st;
    }
    pIn = (const u8 *)pSrc;
    pOut = (volatile u8 *)pVa;
    for (iByte = 0; iByte < cb; iByte++) {
        pOut[iByte] = pIn[iByte];
    }
    host_soft_inc(&g_u32HostBarMemcpy);
    return UDX_OK;
#endif
}

udx_status_t
udx_host_bar_memcpy_from(struct udx_pci_dev *pPdev, int nBar, u64 u64Off,
                         void *pDst, size_t cb)
{
#if !defined(UDX_HOST_LIBC)
    (void)pPdev;
    (void)nBar;
    (void)u64Off;
    (void)pDst;
    (void)cb;
    host_soft_inc(&g_u32HostBarNosys);
    return UDX_ERR_NOSYS;
#else
    void *pVa;
    udx_status_t st;
    size_t iByte;
    u8 *pOut;
    const volatile u8 *pIn;

    if (pDst == NULL || cb == 0) {
        host_soft_inc(&g_u32HostBarFail);
        return UDX_ERR_INVAL;
    }
    st = host_bar_va(pPdev, nBar, u64Off, (u64)cb, &pVa);
    if (st != UDX_OK) {
        host_soft_inc(&g_u32HostBarFail);
        return st;
    }
    pOut = (u8 *)pDst;
    pIn = (const volatile u8 *)pVa;
    for (iByte = 0; iByte < cb; iByte++) {
        pOut[iByte] = pIn[iByte];
    }
    host_soft_inc(&g_u32HostBarMemcpy);
    return UDX_OK;
#endif
}

/* pci inject / remove live in pci.c — declared in host.h */

/*
 * Soft DDI host init — first path greppable PASS.
 * Idempotent; observation only; never gates skeleton PASS.
 *
 * greppable: udx: soft ddi host note PASS
 */
void
udx_host_soft_init(void)
{
    host_soft_inc(&g_u32DdiInitCalls);
    if (g_fDdiHostInited != 0) {
        return;
    }
    g_fDdiHostInited = 1;
    /*
     * Grep: udx: soft ddi host note PASS
     * Soft types (BDF/BAR/window_grant) ready for demos; product mint OPEN.
     * No GPL; dual MIT OR Apache-2.0; no live cap grant.
     */
    host_soft_emit("udx: soft ddi host note PASS "
                   "abi_first=1 dual_license=1 gpl=0 "
                   "bdf=1 bar=1 dma_grant=soft_fail "
                   "lifecycle=probe/remove/quiesce "
                   "product_mint=0 soft=1 hard_gate=0\n");
}

int
udx_host_soft_ready(void)
{
    return (g_fDdiHostInited != 0) ? 1 : 0;
}

/*
 * Soft note for future DDI attachment (ABI-first pivot).
 * UDX hosts out-of-tree dual-license drivers; no GPL.
 * Observation only; never gates skeleton PASS or freestanding path.
 *
 * greppable: udx: soft ddi-ready
 */
void
udx_host_soft_note_ddi(void)
{
    /* Ensure first-host-init PASS lamp has fired. */
    udx_host_soft_init();
    /*
     * Grep: udx: soft ddi-ready
     * abi_first=1 — product order is ABI + virtio, then DDI caps.
     * out_of_tree=1 dual_license=1 gpl=0 — host surface for MIT/Apache drivers.
     */
    host_soft_emit("udx: soft ddi-ready "
                   "abi_first=1 out_of_tree=1 dual_license=1 gpl=0 "
                   "host=udx ddi_attach=future soft=1 hard_gate=0\n");
}

/*
 * Soft lifecycle catalog — probe / remove / quiesce (DDI spirit).
 * Documentation lamp only; does not attach or detach devices.
 *
 * greppable: udx: soft ddi lifecycle note
 */
void
udx_host_soft_lifecycle_note(void)
{
    host_soft_inc(&g_u32DdiLifecycleN);
    udx_host_soft_init();
    /*
     * Grep: udx: soft ddi lifecycle note
     * probe → live attach; quiesce → stop DMA/IRQ/work; remove → free resources.
     * Product: process death ⇒ kernel Phase-A revoke (not claimed here).
     */
    host_soft_emit("udx: soft ddi lifecycle note "
                   "probe=attach remove=detach quiesce=stop_dma_mask_irq "
                   "order=quiesce_before_remove "
                   "crash_revoke=product_later "
                   "soft=1 product=0 gpl=0\n");
}

udx_status_t
udx_host_soft_bdf_from_pdev(const struct udx_pci_dev *pPdev,
                            struct udx_host_bdf *pOut)
{
    if (pPdev == NULL || pOut == NULL) {
        return UDX_ERR_INVAL;
    }
    pOut->u8Bus = pPdev->u8Bus;
    pOut->u8Slot = udx_pci_slot(pPdev);
    pOut->u8Func = udx_pci_func(pPdev);
    pOut->u8Pad = 0;
    host_soft_inc(&g_u32DdiBdfSnap);
    return UDX_OK;
}

udx_status_t
udx_host_soft_bar_snapshot(const struct udx_pci_dev *pPdev, int nBar,
                           struct udx_host_bar *pOut)
{
    if (pPdev == NULL || pOut == NULL || nBar < 0 || nBar > 5) {
        return UDX_ERR_INVAL;
    }
    pOut->u8Index = (u8)nBar;
    pOut->u8Mem = pPdev->aBarIsMem[nBar] ? 1u : 0u;
    pOut->u8Prefetch = 0; /* soft inventory; inject path does not track */
    pOut->u8Pad = 0;
    pOut->u64Pa = pPdev->aBarPhys[nBar];
    pOut->u64Cb = pPdev->aBarLen[nBar];
    host_soft_inc(&g_u32DdiBarSnap);
    return UDX_OK;
}

udx_status_t
udx_host_soft_bar_snapshot_all(const struct udx_pci_dev *pPdev,
                               struct udx_host_bar *aOut, u32 *pu32CountOut)
{
    int nBar;
    u32 u32Count;

    if (pPdev == NULL || aOut == NULL) {
        return UDX_ERR_INVAL;
    }
    u32Count = 0;
    for (nBar = 0; nBar < (int)UDX_HOST_SOFT_BAR_MAX; nBar++) {
        if (udx_host_soft_bar_snapshot(pPdev, nBar, &aOut[nBar]) != UDX_OK) {
            return UDX_ERR_INVAL;
        }
        if (aOut[nBar].u64Cb != 0) {
            if (u32Count < 0xffffffffu) {
                u32Count++;
            }
        }
    }
    if (pu32CountOut != NULL) {
        *pu32CountOut = u32Count;
    }
    return UDX_OK;
}

/*
 * Soft DMA window grant request — always not-granted until product.
 *
 * greppable: udx: soft ddi dma not-granted
 */
udx_status_t
udx_host_soft_dma_window_request(const struct udx_host_bdf *pBdf,
                                 u64 u64PaBase, u64 u64Cb, u8 u8Rw,
                                 struct udx_host_window_grant *pGrantOut)
{
    host_soft_inc(&g_u32DdiDmaReq);
    udx_host_soft_init();

    if (pBdf == NULL || u64Cb == 0) {
        host_soft_inc(&g_u32DdiDmaReject);
        if (pGrantOut != NULL) {
            pGrantOut->bdf.u8Bus = 0;
            pGrantOut->bdf.u8Slot = 0;
            pGrantOut->bdf.u8Func = 0;
            pGrantOut->bdf.u8Pad = 0;
            pGrantOut->u64PaBase = 0;
            pGrantOut->u64Cb = 0;
            pGrantOut->u8Ok = 0;
            pGrantOut->u8Rw = 0;
            pGrantOut->u8Pad[0] = 0;
            pGrantOut->u8Pad[1] = 0;
            pGrantOut->u32WinId = 0xffffffffu;
        }
        return UDX_ERR_INVAL;
    }
    /* Overflow: pa+cb must not wrap. */
    if (u64PaBase + u64Cb < u64PaBase) {
        host_soft_inc(&g_u32DdiDmaReject);
        if (pGrantOut != NULL) {
            pGrantOut->bdf = *pBdf;
            pGrantOut->u64PaBase = u64PaBase;
            pGrantOut->u64Cb = u64Cb;
            pGrantOut->u8Ok = 0;
            pGrantOut->u8Rw = u8Rw;
            pGrantOut->u8Pad[0] = 0;
            pGrantOut->u8Pad[1] = 0;
            pGrantOut->u32WinId = 0xffffffffu;
        }
        return UDX_ERR_INVAL;
    }

    host_soft_inc(&g_u32DdiDmaReject);
    if (pGrantOut != NULL) {
        pGrantOut->bdf = *pBdf;
        pGrantOut->u64PaBase = u64PaBase;
        pGrantOut->u64Cb = u64Cb;
        pGrantOut->u8Ok = 0; /* not granted — product mint OPEN */
        pGrantOut->u8Rw = u8Rw;
        pGrantOut->u8Pad[0] = 0;
        pGrantOut->u8Pad[1] = 0;
        pGrantOut->u32WinId = 0xffffffffu;
    }

    /*
     * Grep: udx: soft ddi dma not-granted
     * Soft fail until devmgr mints real DMA window caps (docs/DDI_SOFT.md).
     */
    host_soft_emit("udx: soft ddi dma not-granted "
                   "bus=%u slot=%u func=%u pa=0x%llx cb=0x%llx rw=0x%x "
                   "ok=0 win_id=0xffffffff product_mint=0 soft=1\n",
                   (unsigned)pBdf->u8Bus, (unsigned)pBdf->u8Slot,
                   (unsigned)pBdf->u8Func,
                   (unsigned long long)u64PaBase,
                   (unsigned long long)u64Cb,
                   (unsigned)u8Rw);

    return UDX_ERR_NOSYS;
}

u32
udx_host_soft_init_calls(void)
{
    return g_u32DdiInitCalls;
}

u32
udx_host_soft_dma_requests(void)
{
    return g_u32DdiDmaReq;
}

u32
udx_host_soft_dma_rejects(void)
{
    return g_u32DdiDmaReject;
}

/*
 * Soft DDI bind lamps (freestanding GJ_SYS_DDI path).
 * greppable: udx: soft ddi bind PASS
 * greppable: udx: soft ddi bind SKIP
 */
#if (!defined(UDX_HOST_LIBC)) || defined(GJ_FREESTANDING)
static void
host_soft_ddi_bind_pass(u16 u16Vendor, u16 u16Device, u64 u64Bar0Pa,
                        u64 u64Bar0Len)
{
    host_soft_inc(&g_u32DdiBindPass);
    udx_host_soft_init();
    /*
     * Grep: udx: soft ddi bind PASS
     * Granted BAR0 PA window-registered; ioremap uses that PA.
     * Soft ≠ product multi-server / VT-d mint.
     */
    host_soft_emit("udx: soft ddi bind PASS "
                   "vend=%04x dev=%04x bar0_pa=0x%llx bar0_len=0x%llx "
                   "sys=GJ_SYS_DDI ioremap=granted_pa "
                   "soft=1 product_mint=0 gpl=0\n",
                   (unsigned)u16Vendor, (unsigned)u16Device,
                   (unsigned long long)u64Bar0Pa,
                   (unsigned long long)u64Bar0Len);
    /* Also via printk so freestanding binaries carry greppable text. */
    udx_printk("udx: soft ddi bind PASS vend=%04x dev=%04x "
               "bar0_pa=%llx soft=1\n",
               (unsigned)u16Vendor, (unsigned)u16Device,
               (unsigned long long)u64Bar0Pa);
}
#endif

static void
host_soft_ddi_bind_skip(const char *szWhy)
{
    host_soft_inc(&g_u32DdiBindSkip);
    udx_host_soft_init();
    if (szWhy == NULL) {
        szWhy = "unknown";
    }
    /*
     * Grep: udx: soft ddi bind SKIP
     * Syscall unavailable, no match, or host-linux soft fallback.
     * Inject path remains for host-linux tests (soft ≠ product).
     */
    host_soft_emit("udx: soft ddi bind SKIP why=%s "
                   "sys=GJ_SYS_DDI soft_fallback=inject_host_linux "
                   "g752_targets=10ec:8168,8086:a12f "
                   "soft=1 product_mint=0 gpl=0\n",
                   szWhy);
    udx_printk("udx: soft ddi bind SKIP why=%s soft=1\n", szWhy);
}

#if (!defined(UDX_HOST_LIBC)) || defined(GJ_FREESTANDING)
/**
 * Soft DDI syscall wrapper. Kernel may return -ENOSYS until product.
 * Linux-errno-shaped negatives are treated as failure.
 */
static long
host_ddi_syscall4(long a0, long a1, long a2, long a3)
{
    long ret;

    host_soft_inc(&g_u32DdiBindSys);
    ret = udx_gj_syscall4(UDX_GJ_SYS_DDI, a0, a1, a2, a3);
    if (ret < 0) {
        host_soft_inc(&g_u32DdiBindSysFail);
    }
    return ret;
}

static long
host_ddi_syscall3(long a0, long a1, long a2)
{
    long ret;

    host_soft_inc(&g_u32DdiBindSys);
    ret = udx_gj_syscall3(UDX_GJ_SYS_DDI, a0, a1, a2);
    if (ret < 0) {
        host_soft_inc(&g_u32DdiBindSysFail);
    }
    return ret;
}

/**
 * Kernel ABI path: SCAN → GET by index → match vend:dev → OPEN → MAP_BAR0.
 * MAP_BAR arg3 = &udx_ddi_map_note (matches kernel gj_ddi_map_note).
 */
static udx_status_t
host_ddi_open_map_install(u16 u16Vendor, u16 u16Device,
                          struct udx_pci_dev **ppOut)
{
    struct udx_ddi_dev_info info;
    struct udx_ddi_map_note note;
    long ret;
    long nDev;
    long i;
    long h;
    void *pBar0Va;
    udx_status_t st;
    u32 iWord;
    u8 *pBytes;
    u64 aBarPhys[6];
    u64 aBarLen[6];
    u8  aBarIsMem[6];
    u8  u8Devfn;

    nDev = host_ddi_syscall3(UDX_DDI_OP_SCAN, 0, 0);
    if (nDev < 0) {
        return UDX_ERR_NOSYS;
    }
    if (nDev == 0) {
        return UDX_ERR_NODEV;
    }

    for (i = 0; i < nDev && i < (long)UDX_DDI_SCAN_MAX; i++) {
        pBytes = (u8 *)(void *)&info;
        for (iWord = 0; iWord < (u32)sizeof(info); iWord++) {
            pBytes[iWord] = 0;
        }
        ret = host_ddi_syscall4(UDX_DDI_OP_GET, i, (long)(uintptr_t)&info, 0);
        if (ret < 0) {
            continue;
        }
        if (info.u16Vend != u16Vendor || info.u16Dev != u16Device) {
            continue;
        }

        h = host_ddi_syscall3(UDX_DDI_OP_OPEN, i, 0);
        if (h <= 0) {
            return UDX_ERR_NODEV;
        }

        pBytes = (u8 *)(void *)&note;
        for (iWord = 0; iWord < (u32)sizeof(note); iWord++) {
            pBytes[iWord] = 0;
        }
        pBar0Va = NULL;
        aBarPhys[0] = info.u64Bar0Pa;
        aBarLen[0] = info.u64Bar0Cb;
        aBarPhys[1] = info.u64Bar1Pa;
        aBarLen[1] = info.u64Bar1Cb;
        aBarPhys[2] = info.u64Bar2Pa;
        aBarLen[2] = info.u64Bar2Cb;
        aBarPhys[3] = info.u64Bar3Pa;
        aBarLen[3] = info.u64Bar3Cb;
        aBarPhys[4] = info.u64Bar4Pa;
        aBarLen[4] = info.u64Bar4Cb;
        aBarPhys[5] = info.u64Bar5Pa;
        aBarLen[5] = info.u64Bar5Cb;
        for (iWord = 0; iWord < 6u; iWord++) {
            aBarIsMem[iWord] = (aBarLen[iWord] != 0) ? 1u : 0u;
        }

        if (info.u64Bar0Pa != 0 && info.u64Bar0Cb != 0) {
            ret = host_ddi_syscall4(UDX_DDI_OP_MAP_BAR, h, 0,
                                    (long)(uintptr_t)&note);
            if (ret >= 0 && note.u8Ok != 0 && note.u64Va != 0) {
                pBar0Va = (void *)(uintptr_t)note.u64Va;
                if (note.u64Pa != 0) {
                    aBarPhys[0] = note.u64Pa;
                }
                if (note.u64Cb != 0) {
                    aBarLen[0] = note.u64Cb;
                }
            } else if (ret > 0) {
                /* Positive i64 = user VA when low enough to fit. */
                pBar0Va = (void *)(uintptr_t)(u64)ret;
            }
        }

        u8Devfn = (u8)((info.u8Slot << 3) | (info.u8Func & 7u));
        st = udx_host_install_granted_pci(
            info.u16Vend, info.u16Dev,
            0xffffu, 0xffffu, 0,
            info.u8Bus, u8Devfn, 0,
            aBarPhys, aBarLen, aBarIsMem,
            pBar0Va, ppOut);
        if (st != UDX_OK) {
            return st;
        }
        host_soft_ddi_bind_pass(info.u16Vend, info.u16Dev,
                                aBarPhys[0], aBarLen[0]);
        return UDX_OK;
    }
    return UDX_ERR_NODEV;
}
#endif /* freestanding DDI */

udx_status_t
udx_host_bind_scan(void)
{
    host_soft_inc(&g_u32DdiBindEnter);
    udx_host_soft_init();

#if defined(UDX_HOST_LIBC) && !defined(GJ_FREESTANDING)
    /*
     * Host-linux: no GJ_SYS_DDI. Soft SKIP; inject remains for lab tests.
     * Soft fallback ≠ product.
     */
    host_soft_ddi_bind_skip("host_linux_no_sys_ddi");
    return UDX_ERR_NOSYS;
#else
    {
        struct udx_ddi_dev_info info;
        long ret;
        long iEnt;
        long nDev;
        u32 u32Ok;
        u32 iByte;
        u8 *pBytes;

        nDev = host_ddi_syscall3(UDX_DDI_OP_SCAN, 0, 0);
        if (nDev < 0) {
            host_soft_ddi_bind_skip("sys_ddi_scan_enosys");
            return UDX_ERR_NOSYS;
        }
        if (nDev == 0) {
            host_soft_ddi_bind_skip("sys_ddi_scan_empty");
            return UDX_ERR_NODEV;
        }

        u32Ok = 0;
        for (iEnt = 0; iEnt < nDev && iEnt < (long)UDX_DDI_SCAN_MAX; iEnt++) {
            udx_status_t st;
            struct udx_pci_dev *pPdev;

            pBytes = (u8 *)(void *)&info;
            for (iByte = 0; iByte < (u32)sizeof(info); iByte++) {
                pBytes[iByte] = 0;
            }
            ret = host_ddi_syscall4(UDX_DDI_OP_GET, iEnt,
                                    (long)(uintptr_t)&info, 0);
            if (ret < 0 || info.u16Vend == 0) {
                continue;
            }
            pPdev = NULL;
            st = host_ddi_open_map_install(info.u16Vend, info.u16Dev, &pPdev);
            if (st == UDX_OK) {
                if (u32Ok < 0xffffffffu) {
                    u32Ok++;
                }
            }
        }

        if (u32Ok == 0) {
            host_soft_ddi_bind_skip("sys_ddi_scan_no_install");
            return UDX_ERR_NODEV;
        }
        return UDX_OK;
    }
#endif
}

udx_status_t
udx_host_bind_by_id(u16 u16Vendor, u16 u16Device,
                    struct udx_pci_dev **ppOut)
{
    host_soft_inc(&g_u32DdiBindEnter);
    udx_host_soft_init();

    if (ppOut != NULL) {
        *ppOut = NULL;
    }

#if defined(UDX_HOST_LIBC) && !defined(GJ_FREESTANDING)
    /*
     * Host-linux soft fallback: inject path stays available for tests.
     * Document first G752 targets in SKIP why for greppable inventory.
     */
    (void)u16Vendor;
    (void)u16Device;
    host_soft_ddi_bind_skip("host_linux_use_inject");
    return UDX_ERR_NOSYS;
#else
    {
        udx_status_t st;

        st = host_ddi_open_map_install(u16Vendor, u16Device, ppOut);
        if (st != UDX_OK) {
            host_soft_ddi_bind_skip("sys_ddi_open_or_map");
            return st;
        }
        return UDX_OK;
    }
#endif
}

/* Soft DDI deepen (soft ≠ product):
 * greppable: udx: soft ddi host note PASS
 * greppable: udx: soft ddi-ready
 * greppable: udx: soft ddi lifecycle note
 * greppable: udx: soft ddi dma not-granted
 * greppable: udx: soft ddi bind PASS
 * greppable: udx: soft ddi bind SKIP
 * Product lamps 0; bar3 OPEN; no GPL.
 * G752 first bind targets: 10ec:8168, 8086:a12f.
 */
