/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Host MMIO window table + inject helpers (UDX_HOST_LIBC).
 * Window table is also available freestanding for granted maps.
 */
#include "udx_internal.h"

#include <udx/host.h>
#include <udx/irq.h>
#include <udx/udx.h>

#if defined(UDX_HOST_LIBC)
#include <stdlib.h>
#include <string.h>
#endif

#define UDX_HOST_WIN_MAX 64

struct udx_host_window {
    u64   u64Phys;
    u64   u64Len;
    void *pVa;
    u8    u8Used;
};

static struct udx_host_window g_aWin[UDX_HOST_WIN_MAX];

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

    if (pVa == NULL || u64Len == 0) {
        return;
    }
    /* Overflow: phys+len must not wrap. */
    if (u64Phys + u64Len < u64Phys) {
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
            return;
        }
        if (!g_aWin[iWin].u8Used && iFree == UDX_HOST_WIN_MAX) {
            iFree = iWin;
        }
    }
    if (iFree >= UDX_HOST_WIN_MAX) {
        udx_printk("udx: host window table full\n");
        return;
    }
    g_aWin[iFree].u64Phys = u64Phys;
    g_aWin[iFree].u64Len = u64Len;
    g_aWin[iFree].pVa = pVa;
    g_aWin[iFree].u8Used = 1;
}

void
udx_host_window_unregister(u64 u64Phys)
{
    u32 iWin;

    for (iWin = 0; iWin < UDX_HOST_WIN_MAX; iWin++) {
        if (g_aWin[iWin].u8Used && g_aWin[iWin].u64Phys == u64Phys) {
            g_aWin[iWin].u8Used = 0;
            g_aWin[iWin].pVa = NULL;
            g_aWin[iWin].u64Len = 0;
            g_aWin[iWin].u64Phys = 0;
            return;
        }
    }
}

void *
udx_host_window_lookup(u64 u64Phys, u64 u64Len, u64 *pu64OffOut)
{
    u32 iWin;

    if (u64Len == 0) {
        return NULL;
    }
    if (u64Phys + u64Len < u64Phys) {
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
            return (u8 *)g_aWin[iWin].pVa + (u64Phys - u64Base);
        }
    }
    return NULL;
}

void
udx_host_fire_irq(int nIrq)
{
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
    return UDX_ERR_NOSYS;
#else
    void *pVa;
    udx_status_t st;

    st = host_bar_va(pPdev, nBar, u64Off, 4, &pVa);
    if (st != UDX_OK) {
        return st;
    }
    *(volatile u32 *)pVa = u32Val;
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
    return UDX_ERR_NOSYS;
#else
    void *pVa;
    udx_status_t st;

    if (pu32Val == NULL) {
        return UDX_ERR_INVAL;
    }
    st = host_bar_va(pPdev, nBar, u64Off, 4, &pVa);
    if (st != UDX_OK) {
        return st;
    }
    *pu32Val = *(volatile u32 *)pVa;
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
    return UDX_ERR_NOSYS;
#else
    void *pVa;
    udx_status_t st;

    st = host_bar_va(pPdev, nBar, u64Off, 2, &pVa);
    if (st != UDX_OK) {
        return st;
    }
    *(volatile u16 *)pVa = u16Val;
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
    return UDX_ERR_NOSYS;
#else
    void *pVa;
    udx_status_t st;

    if (pu16Val == NULL) {
        return UDX_ERR_INVAL;
    }
    st = host_bar_va(pPdev, nBar, u64Off, 2, &pVa);
    if (st != UDX_OK) {
        return st;
    }
    *pu16Val = *(volatile u16 *)pVa;
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
    return UDX_ERR_NOSYS;
#else
    void *pVa;
    udx_status_t st;

    st = host_bar_va(pPdev, nBar, u64Off, 1, &pVa);
    if (st != UDX_OK) {
        return st;
    }
    *(volatile u8 *)pVa = u8Val;
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
    return UDX_ERR_NOSYS;
#else
    void *pVa;
    udx_status_t st;

    if (pu8Val == NULL) {
        return UDX_ERR_INVAL;
    }
    st = host_bar_va(pPdev, nBar, u64Off, 1, &pVa);
    if (st != UDX_OK) {
        return st;
    }
    *pu8Val = *(volatile u8 *)pVa;
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
    return UDX_ERR_NOSYS;
#else
    void *pVa;
    udx_status_t st;

    st = host_bar_va(pPdev, nBar, u64Off, 8, &pVa);
    if (st != UDX_OK) {
        return st;
    }
    *(volatile u64 *)pVa = u64Val;
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
    return UDX_ERR_NOSYS;
#else
    void *pVa;
    udx_status_t st;

    if (pu64Val == NULL) {
        return UDX_ERR_INVAL;
    }
    st = host_bar_va(pPdev, nBar, u64Off, 8, &pVa);
    if (st != UDX_OK) {
        return st;
    }
    *pu64Val = *(volatile u64 *)pVa;
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
    return UDX_ERR_NOSYS;
#else
    void *pVa;
    udx_status_t st;
    size_t iByte;
    const u8 *pIn;
    volatile u8 *pOut;

    if (pSrc == NULL || cb == 0) {
        return UDX_ERR_INVAL;
    }
    st = host_bar_va(pPdev, nBar, u64Off, (u64)cb, &pVa);
    if (st != UDX_OK) {
        return st;
    }
    pIn = (const u8 *)pSrc;
    pOut = (volatile u8 *)pVa;
    for (iByte = 0; iByte < cb; iByte++) {
        pOut[iByte] = pIn[iByte];
    }
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
    return UDX_ERR_NOSYS;
#else
    void *pVa;
    udx_status_t st;
    size_t iByte;
    u8 *pOut;
    const volatile u8 *pIn;

    if (pDst == NULL || cb == 0) {
        return UDX_ERR_INVAL;
    }
    st = host_bar_va(pPdev, nBar, u64Off, (u64)cb, &pVa);
    if (st != UDX_OK) {
        return st;
    }
    pOut = (u8 *)pDst;
    pIn = (const volatile u8 *)pVa;
    for (iByte = 0; iByte < cb; iByte++) {
        pOut[iByte] = pIn[iByte];
    }
    return UDX_OK;
#endif
}

/* pci inject / remove live in pci.c — declared in host.h */
