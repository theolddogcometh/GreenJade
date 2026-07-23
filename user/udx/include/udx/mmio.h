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

static inline int
udx_mmio_in_range(const struct udx_iomem *pIo, u64 u64Off, u64 u64Width)
{
    if (pIo == NULL || pIo->pVa == NULL || u64Width == 0) {
        return 0;
    }
    if (u64Off > pIo->u64Len) {
        return 0;
    }
    if (u64Width > pIo->u64Len - u64Off) {
        return 0;
    }
    return 1;
}

static inline u32
udx_readl(const struct udx_iomem *pIo, u64 u64Off)
{
    const volatile u32 *p;

    if (!udx_mmio_in_range(pIo, u64Off, 4)) {
        return 0xffffffffu;
    }
    p = (const volatile u32 *)((const volatile u8 *)pIo->pVa + u64Off);
    return *p;
}

static inline void
udx_writel(struct udx_iomem *pIo, u64 u64Off, u32 u32Val)
{
    volatile u32 *p;

    if (!udx_mmio_in_range(pIo, u64Off, 4)) {
        return;
    }
    p = (volatile u32 *)((volatile u8 *)pIo->pVa + u64Off);
    *p = u32Val;
}

static inline u16
udx_readw(const struct udx_iomem *pIo, u64 u64Off)
{
    const volatile u16 *p;

    if (!udx_mmio_in_range(pIo, u64Off, 2)) {
        return 0xffffu;
    }
    p = (const volatile u16 *)((const volatile u8 *)pIo->pVa + u64Off);
    return *p;
}

static inline void
udx_writew(struct udx_iomem *pIo, u64 u64Off, u16 u16Val)
{
    volatile u16 *p;

    if (!udx_mmio_in_range(pIo, u64Off, 2)) {
        return;
    }
    p = (volatile u16 *)((volatile u8 *)pIo->pVa + u64Off);
    *p = u16Val;
}

static inline u8
udx_readb(const struct udx_iomem *pIo, u64 u64Off)
{
    const volatile u8 *p;

    if (!udx_mmio_in_range(pIo, u64Off, 1)) {
        return 0xffu;
    }
    p = (const volatile u8 *)pIo->pVa + u64Off;
    return *p;
}

static inline void
udx_writeb(struct udx_iomem *pIo, u64 u64Off, u8 u8Val)
{
    volatile u8 *p;

    if (!udx_mmio_in_range(pIo, u64Off, 1)) {
        return;
    }
    p = (volatile u8 *)pIo->pVa + u64Off;
    *p = u8Val;
}

static inline u64
udx_readq(const struct udx_iomem *pIo, u64 u64Off)
{
    const volatile u64 *p;

    if (!udx_mmio_in_range(pIo, u64Off, 8)) {
        return 0xffffffffffffffffull;
    }
    p = (const volatile u64 *)((const volatile u8 *)pIo->pVa + u64Off);
    return *p;
}

static inline void
udx_writeq(struct udx_iomem *pIo, u64 u64Off, u64 u64Val)
{
    volatile u64 *p;

    if (!udx_mmio_in_range(pIo, u64Off, 8)) {
        return;
    }
    p = (volatile u64 *)((volatile u8 *)pIo->pVa + u64Off);
    *p = u64Val;
}

/* Linux ioread/iowrite name aliases (same soft path). */
static inline u32 udx_ioread32(const struct udx_iomem *pIo, u64 u64Off)
{
    return udx_readl(pIo, u64Off);
}
static inline void udx_iowrite32(struct udx_iomem *pIo, u64 u64Off, u32 u32Val)
{
    udx_writel(pIo, u64Off, u32Val);
}
static inline u16 udx_ioread16(const struct udx_iomem *pIo, u64 u64Off)
{
    return udx_readw(pIo, u64Off);
}
static inline void udx_iowrite16(struct udx_iomem *pIo, u64 u64Off, u16 u16Val)
{
    udx_writew(pIo, u64Off, u16Val);
}
static inline u8 udx_ioread8(const struct udx_iomem *pIo, u64 u64Off)
{
    return udx_readb(pIo, u64Off);
}
static inline void udx_iowrite8(struct udx_iomem *pIo, u64 u64Off, u8 u8Val)
{
    udx_writeb(pIo, u64Off, u8Val);
}
static inline u64 udx_ioread64(const struct udx_iomem *pIo, u64 u64Off)
{
    return udx_readq(pIo, u64Off);
}
static inline void udx_iowrite64(struct udx_iomem *pIo, u64 u64Off, u64 u64Val)
{
    udx_writeq(pIo, u64Off, u64Val);
}

/**
 * Bulk MMIO helpers (Linux memcpy_fromio / toio / memset_io spirit).
 * Host soft: plain byte copy through volatile-mapped VA.
 */
static inline void
udx_memcpy_fromio(void *pDst, const struct udx_iomem *pIo, u64 u64Off,
                  size_t cb)
{
    const volatile u8 *pSrc;
    u8 *pOut;
    size_t iByte;

    if (pDst == NULL || cb == 0 || !udx_mmio_in_range(pIo, u64Off, (u64)cb)) {
        return;
    }
    pSrc = (const volatile u8 *)pIo->pVa + u64Off;
    pOut = (u8 *)pDst;
    for (iByte = 0; iByte < cb; iByte++) {
        pOut[iByte] = pSrc[iByte];
    }
}

static inline void
udx_memcpy_toio(struct udx_iomem *pIo, u64 u64Off, const void *pSrc,
                size_t cb)
{
    volatile u8 *pDst;
    const u8 *pIn;
    size_t iByte;

    if (pSrc == NULL || cb == 0 || !udx_mmio_in_range(pIo, u64Off, (u64)cb)) {
        return;
    }
    pDst = (volatile u8 *)pIo->pVa + u64Off;
    pIn = (const u8 *)pSrc;
    for (iByte = 0; iByte < cb; iByte++) {
        pDst[iByte] = pIn[iByte];
    }
}

static inline void
udx_memset_io(struct udx_iomem *pIo, u64 u64Off, u8 u8Val, size_t cb)
{
    volatile u8 *pDst;
    size_t iByte;

    if (cb == 0 || !udx_mmio_in_range(pIo, u64Off, (u64)cb)) {
        return;
    }
    pDst = (volatile u8 *)pIo->pVa + u64Off;
    for (iByte = 0; iByte < cb; iByte++) {
        pDst[iByte] = u8Val;
    }
}

/** Compiler/CPU fence after a batch of MMIO writes (posted-write flush intent). */
static inline void
udx_mmio_flush(const struct udx_iomem *pIo)
{
    (void)pIo;
#if defined(__x86_64__) || defined(__i386__)
    __asm__ volatile("mfence" ::: "memory");
#elif defined(__aarch64__)
    __asm__ volatile("dmb sy" ::: "memory");
#else
    __atomic_thread_fence(__ATOMIC_SEQ_CST);
#endif
}
