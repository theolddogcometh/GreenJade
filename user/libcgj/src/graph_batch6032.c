/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6032: GPU memory heap budget reserves.
 *
 * Surface (unique symbols):
 *   uint32_t gj_gpu_heap_budget_reserve_mib_6032(void);
 *     - Returns 256 (soft driver/OS device-local reserve MiB).
 *   uint32_t gj_gpu_heap_budget_host_reserve_mib_6032(void);
 *     - Returns 64 (soft host-visible reserve MiB).
 *   uint32_t gj_gpu_heap_budget_compositor_mib_6032(void);
 *     - Returns 128 (soft compositor/display reserve MiB).
 *   uint32_t gj_gpu_heap_budget_reserve_sum_6032(void);
 *     - Returns 448 (256 + 64 + 128 soft combined reserve).
 *   uint32_t __gj_gpu_heap_budget_reserve_mib_6032  (alias)
 *   uint32_t __gj_gpu_heap_budget_host_reserve_mib_6032  (alias)
 *   uint32_t __gj_gpu_heap_budget_compositor_mib_6032  (alias)
 *   uint32_t __gj_gpu_heap_budget_reserve_sum_6032  (alias)
 *   __libcgj_batch6032_marker = "libcgj-batch6032"
 *
 * Exclusive continuum CREATE-ONLY (6031-6040: GPU memory heap budget
 * stubs). Soft reserve constants; distinct from totals in 6031. No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6032_marker[] = "libcgj-batch6032";

#define B6032_RESERVE_MIB       256u
#define B6032_HOST_RESERVE_MIB  64u
#define B6032_COMPOSITOR_MIB    128u
#define B6032_RESERVE_SUM       448u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6032_reserve(void)
{
	return B6032_RESERVE_MIB;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gpu_heap_budget_reserve_mib_6032 - soft device-local driver reserve.
 *
 * Always returns 256 MiB. Product-path constant only.
 */
uint32_t
gj_gpu_heap_budget_reserve_mib_6032(void)
{
	(void)NULL;
	return b6032_reserve();
}

/*
 * gj_gpu_heap_budget_host_reserve_mib_6032 - soft host-visible reserve.
 *
 * Always returns 64 MiB.
 */
uint32_t
gj_gpu_heap_budget_host_reserve_mib_6032(void)
{
	return B6032_HOST_RESERVE_MIB;
}

/*
 * gj_gpu_heap_budget_compositor_mib_6032 - soft compositor display reserve.
 *
 * Always returns 128 MiB.
 */
uint32_t
gj_gpu_heap_budget_compositor_mib_6032(void)
{
	return B6032_COMPOSITOR_MIB;
}

/*
 * gj_gpu_heap_budget_reserve_sum_6032 - soft combined reserve sum.
 *
 * Always returns 448 (256 + 64 + 128).
 */
uint32_t
gj_gpu_heap_budget_reserve_sum_6032(void)
{
	return B6032_RESERVE_SUM;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_gpu_heap_budget_reserve_mib_6032(void)
    __attribute__((alias("gj_gpu_heap_budget_reserve_mib_6032")));

uint32_t __gj_gpu_heap_budget_host_reserve_mib_6032(void)
    __attribute__((alias("gj_gpu_heap_budget_host_reserve_mib_6032")));

uint32_t __gj_gpu_heap_budget_compositor_mib_6032(void)
    __attribute__((alias("gj_gpu_heap_budget_compositor_mib_6032")));

uint32_t __gj_gpu_heap_budget_reserve_sum_6032(void)
    __attribute__((alias("gj_gpu_heap_budget_reserve_sum_6032")));
