/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6034: GPU memory heap budget usable calc.
 *
 * Surface (unique symbols):
 *   uint32_t gj_gpu_heap_budget_usable_mib_6034(uint32_t total, uint32_t reserve);
 *     - Return total - reserve when total >= reserve, else 0.
 *   uint32_t gj_gpu_heap_budget_usable2_mib_6034(uint32_t total,
 *                                               uint32_t r0, uint32_t r1);
 *     - Subtract two reserves from total; 0 if either exceeds remaining.
 *   uint32_t gj_gpu_heap_budget_default_usable_6034(void);
 *     - Returns 1792 (soft 2048 - 256 default usable MiB).
 *   uint32_t gj_gpu_heap_budget_usable_ready_6034(void);
 *     - Returns 1 (soft usable-calc path ready lamp).
 *   uint32_t __gj_gpu_heap_budget_usable_mib_6034  (alias)
 *   uint32_t __gj_gpu_heap_budget_usable2_mib_6034  (alias)
 *   uint32_t __gj_gpu_heap_budget_default_usable_6034  (alias)
 *   uint32_t __gj_gpu_heap_budget_usable_ready_6034  (alias)
 *   __libcgj_batch6034_marker = "libcgj-batch6034"
 *
 * Exclusive continuum CREATE-ONLY (6031-6040: GPU memory heap budget
 * stubs). Usable arithmetic; distinct from gj_vram_budget_usable_*_5605.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6034_marker[] = "libcgj-batch6034";

#define B6034_DEFAULT_USABLE  1792u
#define B6034_READY           1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6034_usable(uint32_t u32Total, uint32_t u32Reserve)
{
	if (u32Total < u32Reserve) {
		return 0u;
	}
	return u32Total - u32Reserve;
}

static uint32_t
b6034_usable2(uint32_t u32Total, uint32_t u32R0, uint32_t u32R1)
{
	uint32_t u32Left;

	if (u32Total < u32R0) {
		return 0u;
	}
	u32Left = u32Total - u32R0;
	if (u32Left < u32R1) {
		return 0u;
	}
	return u32Left - u32R1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gpu_heap_budget_usable_mib_6034 - soft usable after one reserve.
 *
 * total:   budget total in MiB
 * reserve: reserved MiB
 *
 * Returns total - reserve when total >= reserve, else 0.
 */
uint32_t
gj_gpu_heap_budget_usable_mib_6034(uint32_t u32Total, uint32_t u32Reserve)
{
	(void)NULL;
	return b6034_usable(u32Total, u32Reserve);
}

/*
 * gj_gpu_heap_budget_usable2_mib_6034 - soft usable after two reserves.
 *
 * total: budget total in MiB
 * r0:    first reserve MiB
 * r1:    second reserve MiB
 *
 * Returns total - r0 - r1 when both fit, else 0.
 */
uint32_t
gj_gpu_heap_budget_usable2_mib_6034(uint32_t u32Total, uint32_t u32R0,
                                    uint32_t u32R1)
{
	return b6034_usable2(u32Total, u32R0, u32R1);
}

/*
 * gj_gpu_heap_budget_default_usable_6034 - soft default usable budget.
 *
 * Always returns 1792 (2048 - 256 product-path default).
 */
uint32_t
gj_gpu_heap_budget_default_usable_6034(void)
{
	return B6034_DEFAULT_USABLE;
}

/*
 * gj_gpu_heap_budget_usable_ready_6034 - soft usable-calc ready lamp.
 *
 * Always returns 1. Compile-time readiness tag only.
 */
uint32_t
gj_gpu_heap_budget_usable_ready_6034(void)
{
	return B6034_READY;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_gpu_heap_budget_usable_mib_6034(uint32_t u32Total,
                                              uint32_t u32Reserve)
    __attribute__((alias("gj_gpu_heap_budget_usable_mib_6034")));

uint32_t __gj_gpu_heap_budget_usable2_mib_6034(uint32_t u32Total,
                                               uint32_t u32R0, uint32_t u32R1)
    __attribute__((alias("gj_gpu_heap_budget_usable2_mib_6034")));

uint32_t __gj_gpu_heap_budget_default_usable_6034(void)
    __attribute__((alias("gj_gpu_heap_budget_default_usable_6034")));

uint32_t __gj_gpu_heap_budget_usable_ready_6034(void)
    __attribute__((alias("gj_gpu_heap_budget_usable_ready_6034")));
