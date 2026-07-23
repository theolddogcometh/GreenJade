/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Internal UDX helpers (not for driver .c).
 */
#pragma once

#include <udx/types.h>

/* Work queue: drain pending items (called from udx_run). */
void udx_work_flush(void);

/*
 * Host MMIO windows: phys → host VA for sim / granted maps.
 * Full GreenJade: filled when process maps MMIO FRAME caps.
 * Also used freestanding when a grant registers a window.
 */
void  udx_host_window_register(u64 u64Phys, void *pVa, u64 u64Len);
void  udx_host_window_unregister(u64 u64Phys);
void *udx_host_window_lookup(u64 u64Phys, u64 u64Len, u64 *pu64OffOut);

/* True while udx_run should keep looping. */
u32 udx_core_should_run(void);

/*
 * Freestanding static pools (no libc heap).
 * Host builds use malloc/calloc instead.
 */
#if !defined(UDX_HOST_LIBC)

#define UDX_FS_DMA_SLOTS     8u
#define UDX_FS_DMA_SLOT_CB   4096u
#define UDX_FS_IOMEM_SLOTS   16u

struct udx_fs_dma_slot {
    u8     aBytes[UDX_FS_DMA_SLOT_CB];
    u8     u8Used;
    size_t cbAlloc;
};

#endif /* !UDX_HOST_LIBC */

/*
 * Freestanding GJ syscalls for the driver-host shape.
 * Numbers match kernel GJ_SYS_* (user/libgj/include/gj/syscalls.h).
 * Host (UDX_HOST_LIBC) builds never include these helpers.
 *
 * SysV x86_64 SYSCALL: nr=rax; a0..a5 → rdi,rsi,rdx,r10,r8,r9.
 */
#if !defined(UDX_HOST_LIBC)

#define UDX_GJ_SYS_PLATFORM_INFO  98
#define UDX_GJ_SYS_NOTIFY_WAIT    99
#define UDX_GJ_PLAT_IOMMU_GRANT   5

static inline long
udx_gj_syscall3(long nNr, long a0, long a1, long a2)
{
    long ret;

    __asm__ volatile(
        "mov %1, %%rax\n\t"
        "mov %2, %%rdi\n\t"
        "mov %3, %%rsi\n\t"
        "mov %4, %%rdx\n\t"
        "syscall\n\t"
        "mov %%rax, %0"
        : "=r"(ret)
        : "r"(nNr), "r"(a0), "r"(a1), "r"(a2)
        : "rax", "rdi", "rsi", "rdx", "rcx", "r11", "memory");
    return ret;
}

static inline long
udx_gj_syscall4(long nNr, long a0, long a1, long a2, long a3)
{
    long ret;

    __asm__ volatile(
        "mov %1, %%rax\n\t"
        "mov %2, %%rdi\n\t"
        "mov %3, %%rsi\n\t"
        "mov %4, %%rdx\n\t"
        "mov %5, %%r10\n\t"
        "syscall\n\t"
        "mov %%rax, %0"
        : "=r"(ret)
        : "r"(nNr), "r"(a0), "r"(a1), "r"(a2), "r"(a3)
        : "rax", "rdi", "rsi", "rdx", "r10", "rcx", "r11", "memory");
    return ret;
}

/**
 * Non-blocking NOTIFY_WAIT on MSI-X global (which=0).
 * Badge bit N → IRQ line N for udx_irq_dispatch.
 * Pumped from udx_run only — not from udx_request_irq.
 */
static inline long
udx_gj_notify_wait_poll(void)
{
    /* which=0, mask=~0, block=0 */
    return udx_gj_syscall3(UDX_GJ_SYS_NOTIFY_WAIT, 0, (long)-1, 0);
}

/**
 * IOMMU window grant: PLATFORM_INFO(op=5, bdf, pa, size).
 */
static inline long
udx_gj_iommu_grant(u32 u32Bdf, udx_dma_addr_t dma, size_t cbSize)
{
    return udx_gj_syscall4(UDX_GJ_SYS_PLATFORM_INFO,
                           UDX_GJ_PLAT_IOMMU_GRANT,
                           (long)u32Bdf,
                           (long)dma,
                           (long)cbSize);
}

#endif /* !UDX_HOST_LIBC */
