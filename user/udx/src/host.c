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

void
udx_host_window_register(u64 u64Phys, void *pVa, u64 u64Len)
{
    u32 iWin;

    if (pVa == NULL || u64Len == 0) {
        return;
    }
    for (iWin = 0; iWin < UDX_HOST_WIN_MAX; iWin++) {
        if (!g_aWin[iWin].u8Used) {
            g_aWin[iWin].u64Phys = u64Phys;
            g_aWin[iWin].u64Len = u64Len;
            g_aWin[iWin].pVa = pVa;
            g_aWin[iWin].u8Used = 1;
            return;
        }
    }
    udx_printk("udx: host window table full\n");
}

void
udx_host_window_unregister(u64 u64Phys)
{
    u32 iWin;

    for (iWin = 0; iWin < UDX_HOST_WIN_MAX; iWin++) {
        if (g_aWin[iWin].u8Used && g_aWin[iWin].u64Phys == u64Phys) {
            g_aWin[iWin].u8Used = 0;
            g_aWin[iWin].pVa = NULL;
            return;
        }
    }
}

void *
udx_host_window_lookup(u64 u64Phys, u64 u64Len, u64 *pu64OffOut)
{
    u32 iWin;

    for (iWin = 0; iWin < UDX_HOST_WIN_MAX; iWin++) {
        u64 u64Base;
        u64 u64End;
        u64 u64ReqEnd;

        if (!g_aWin[iWin].u8Used) {
            continue;
        }
        u64Base = g_aWin[iWin].u64Phys;
        u64End = u64Base + g_aWin[iWin].u64Len;
        u64ReqEnd = u64Phys + u64Len;
        if (u64Phys >= u64Base && u64ReqEnd <= u64End && u64ReqEnd >= u64Phys) {
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

/*
 * Host BAR poke helpers — implemented here so skeleton main does not
 * need soft-state pointers to raise a fake status bit.
 * Needs pci bound backend (inject); freestanding returns NOSYS.
 */
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
    u64 u64Phys;
    u64 u64Len;
    void *pVa;
    volatile u32 *pReg;

    if (pPdev == NULL || nBar < 0 || nBar > 5) {
        return UDX_ERR_INVAL;
    }
    u64Phys = pPdev->aBarPhys[nBar];
    u64Len = pPdev->aBarLen[nBar];
    if (u64Phys == 0 || u64Len == 0 || u64Off + 4 > u64Len) {
        return UDX_ERR_INVAL;
    }
    pVa = udx_host_window_lookup(u64Phys, u64Len, NULL);
    if (pVa == NULL) {
        return UDX_ERR_NODEV;
    }
    pReg = (volatile u32 *)((u8 *)pVa + (size_t)u64Off);
    *pReg = u32Val;
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
    u64 u64Phys;
    u64 u64Len;
    void *pVa;
    volatile u32 *pReg;

    if (pPdev == NULL || pu32Val == NULL || nBar < 0 || nBar > 5) {
        return UDX_ERR_INVAL;
    }
    u64Phys = pPdev->aBarPhys[nBar];
    u64Len = pPdev->aBarLen[nBar];
    if (u64Phys == 0 || u64Len == 0 || u64Off + 4 > u64Len) {
        return UDX_ERR_INVAL;
    }
    pVa = udx_host_window_lookup(u64Phys, u64Len, NULL);
    if (pVa == NULL) {
        return UDX_ERR_NODEV;
    }
    pReg = (volatile u32 *)((u8 *)pVa + (size_t)u64Off);
    *pu32Val = *pReg;
    return UDX_OK;
#endif
}

/* pci inject lives in pci.c — rescan declared in host.h */
