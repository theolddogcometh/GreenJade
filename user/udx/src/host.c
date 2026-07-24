/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Host MMIO window table + inject helpers (UDX_HOST_LIBC).
 * Window table is also available freestanding for granted maps.
 *
 * Soft inventory (Wave 112 exclusive deepen; this unit only) —
 * greppable "udx: host soft …":
 *   udx: host soft inventory …
 *   udx: host soft window …
 *   udx: host soft lookup …
 *   udx: host soft bar …
 *   udx: host soft irq …
 *   udx: host soft path …
 *   udx: host soft wave …
 * Pure observation; never gates host skeleton PASS or freestanding path.
 * Soft ≠ skeleton PASS. greppable: udx: host soft
 *
 * Note: PCI inject/remove live in pci.c (declared in host.h) and carry
 * their own "udx: pci soft …" inventory. This unit owns windows + BAR poke.
 */
#include "udx_internal.h"

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
 * Soft host window / BAR inventory (Wave 112 exclusive deepen).
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
 * Greppable soft host inventory (Wave 112 exclusive deepen).
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
                   "skeleton_gate=0 hard_gate=0 soft=1\n");

    /* Grep: udx: host soft wave */
    host_soft_emit("udx: host soft wave n=%u unit=host exclusive=1 "
                   "prefix=udx:_host_soft deepen=1 "
                   "(soft inventory; never gates skeleton PASS)\n",
                   UDX_HOST_SOFT_WAVE);

    /*
     * Grep: udx: host soft honesty (Wave 112 exclusive deepen).
     * Soft inventory ≠ product multi-server confine.
     */
    host_soft_emit("udx: host soft honesty multi_server=0 confine=0 bar3=0 "
                  "exclusive=1 soft=1 product_kernel=OPEN wave=%u\n",
                  UDX_HOST_SOFT_WAVE);

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

/* Wave 112 soft deepen surfaces (CREATE-ONLY soft ≠ product):
 *   greppable: soft retfragmentangle continuum_toward=25400 soft_ne_product=1 wave=112
 *   greppable: soft retvertexangle exclusive=1 continuum_toward=25400 soft_ne_product=1 wave=112
 * Soft ≠ product complete; product lamps 0; bar3 OPEN.
 */
