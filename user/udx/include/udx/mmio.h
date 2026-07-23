/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * MMIO map + accessors. Host: window table from inject.
 * Freestanding: same API; ioremap uses static iomem pool + host window
 * table when a map grant is registered (no libc heap required).
 */
#pragma once

#include <udx/types.h>

/* Opaque MMIO mapping (≈ ioremap result). */
struct udx_iomem {
    volatile void *pVa;
    u64            u64Len;
    u64            u64Phys; /* optional bookkeeping */
};

/*
 * Map BAR / granted MMIO into host process.
 * Full impl: uses process MMIO/FRAME caps from devmgr.
 */
struct udx_iomem *udx_ioremap(u64 u64Phys, u64 u64Len);
void              udx_iounmap(struct udx_iomem *pIo);

static inline u32
udx_readl(const struct udx_iomem *pIo, u64 u64Off)
{
    const volatile u32 *p;

    if (pIo == NULL || pIo->pVa == NULL || u64Off + 4 > pIo->u64Len) {
        return 0xffffffffu;
    }
    p = (const volatile u32 *)((const volatile u8 *)pIo->pVa + u64Off);
    return *p;
}

static inline void
udx_writel(struct udx_iomem *pIo, u64 u64Off, u32 u32Val)
{
    volatile u32 *p;

    if (pIo == NULL || pIo->pVa == NULL || u64Off + 4 > pIo->u64Len) {
        return;
    }
    p = (volatile u32 *)((volatile u8 *)pIo->pVa + u64Off);
    *p = u32Val;
}

static inline u16
udx_readw(const struct udx_iomem *pIo, u64 u64Off)
{
    const volatile u16 *p;

    if (pIo == NULL || pIo->pVa == NULL || u64Off + 2 > pIo->u64Len) {
        return 0xffffu;
    }
    p = (const volatile u16 *)((const volatile u8 *)pIo->pVa + u64Off);
    return *p;
}

static inline void
udx_writew(struct udx_iomem *pIo, u64 u64Off, u16 u16Val)
{
    volatile u16 *p;

    if (pIo == NULL || pIo->pVa == NULL || u64Off + 2 > pIo->u64Len) {
        return;
    }
    p = (volatile u16 *)((volatile u8 *)pIo->pVa + u64Off);
    *p = u16Val;
}

static inline u8
udx_readb(const struct udx_iomem *pIo, u64 u64Off)
{
    const volatile u8 *p;

    if (pIo == NULL || pIo->pVa == NULL || u64Off + 1 > pIo->u64Len) {
        return 0xffu;
    }
    p = (const volatile u8 *)pIo->pVa + u64Off;
    return *p;
}

static inline void
udx_writeb(struct udx_iomem *pIo, u64 u64Off, u8 u8Val)
{
    volatile u8 *p;

    if (pIo == NULL || pIo->pVa == NULL || u64Off + 1 > pIo->u64Len) {
        return;
    }
    p = (volatile u8 *)pIo->pVa + u64Off;
    *p = u8Val;
}

/** Compiler/CPU fence after a batch of MMIO writes (posted-write flush intent). */
static inline void
udx_mmio_flush(const struct udx_iomem *pIo)
{
    (void)pIo;
    __asm__ volatile("mfence" ::: "memory");
}
