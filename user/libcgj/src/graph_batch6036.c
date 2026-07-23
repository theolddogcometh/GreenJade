/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6036: GPU memory heap budget allocation fit.
 *
 * Surface (unique symbols):
 *   uint32_t gj_gpu_heap_budget_fits_6036(uint32_t free_mib, uint32_t need_mib);
 *     - Return 1 when free_mib >= need_mib, else 0.
 *   uint32_t gj_gpu_heap_budget_fits_aligned_6036(uint32_t free_mib,
 *                                                uint32_t need_mib,
 *                                                uint32_t align_mib);
 *     - Round need up to align_mib (power-of-two), then fit-check.
 *       align_mib 0 or non-pot → treat need unrounded. Overflow → 0.
 *   uint32_t gj_gpu_heap_budget_remain_mib_6036(uint32_t free_mib,
 *                                              uint32_t need_mib);
 *     - Return free - need when free >= need, else 0.
 *   uint32_t gj_gpu_heap_budget_fit_ready_6036(void);
 *     - Returns 1 (soft fit-check path ready lamp).
 *   uint32_t __gj_gpu_heap_budget_fits_6036  (alias)
 *   uint32_t __gj_gpu_heap_budget_fits_aligned_6036  (alias)
 *   uint32_t __gj_gpu_heap_budget_remain_mib_6036  (alias)
 *   uint32_t __gj_gpu_heap_budget_fit_ready_6036  (alias)
 *   __libcgj_batch6036_marker = "libcgj-batch6036"
 *
 * Exclusive continuum CREATE-ONLY (6031-6040: GPU memory heap budget
 * stubs). Allocation fit helpers; no parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6036_marker[] = "libcgj-batch6036";

#define B6036_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6036_fits(uint32_t u32Free, uint32_t u32Need)
{
	return (u32Free >= u32Need) ? 1u : 0u;
}

static uint32_t
b6036_round_up_pot(uint32_t u32Need, uint32_t u32Align)
{
	uint32_t u32Mask;

	if (u32Align == 0u || (u32Align & (u32Align - 1u)) != 0u) {
		return u32Need;
	}
	u32Mask = u32Align - 1u;
	if (u32Need > (UINT32_MAX - u32Mask)) {
		return 0u;
	}
	return (u32Need + u32Mask) & ~u32Mask;
}

static uint32_t
b6036_fits_aligned(uint32_t u32Free, uint32_t u32Need, uint32_t u32Align)
{
	uint32_t u32Rounded;

	u32Rounded = b6036_round_up_pot(u32Need, u32Align);
	if (u32Rounded == 0u && u32Need != 0u) {
		return 0u;
	}
	return (u32Free >= u32Rounded) ? 1u : 0u;
}

static uint32_t
b6036_remain(uint32_t u32Free, uint32_t u32Need)
{
	if (u32Free < u32Need) {
		return 0u;
	}
	return u32Free - u32Need;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gpu_heap_budget_fits_6036 - soft fit check for an allocation.
 *
 * free_mib: free budget MiB
 * need_mib: requested allocation MiB
 *
 * Returns 1 when free >= need, else 0.
 */
uint32_t
gj_gpu_heap_budget_fits_6036(uint32_t u32FreeMib, uint32_t u32NeedMib)
{
	(void)NULL;
	return b6036_fits(u32FreeMib, u32NeedMib);
}

/*
 * gj_gpu_heap_budget_fits_aligned_6036 - soft aligned fit check.
 *
 * free_mib:  free budget MiB
 * need_mib:  requested allocation MiB
 * align_mib: power-of-two alignment in MiB (0 / non-pot → unrounded)
 *
 * Returns 1 when free covers rounded need; 0 on overflow or no fit.
 */
uint32_t
gj_gpu_heap_budget_fits_aligned_6036(uint32_t u32FreeMib, uint32_t u32NeedMib,
                                     uint32_t u32AlignMib)
{
	return b6036_fits_aligned(u32FreeMib, u32NeedMib, u32AlignMib);
}

/*
 * gj_gpu_heap_budget_remain_mib_6036 - soft free remaining after need.
 *
 * Returns free - need when free >= need, else 0.
 */
uint32_t
gj_gpu_heap_budget_remain_mib_6036(uint32_t u32FreeMib, uint32_t u32NeedMib)
{
	return b6036_remain(u32FreeMib, u32NeedMib);
}

/*
 * gj_gpu_heap_budget_fit_ready_6036 - soft fit-check ready lamp.
 *
 * Always returns 1.
 */
uint32_t
gj_gpu_heap_budget_fit_ready_6036(void)
{
	return B6036_READY;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_gpu_heap_budget_fits_6036(uint32_t u32FreeMib, uint32_t u32NeedMib)
    __attribute__((alias("gj_gpu_heap_budget_fits_6036")));

uint32_t __gj_gpu_heap_budget_fits_aligned_6036(uint32_t u32FreeMib,
                                                uint32_t u32NeedMib,
                                                uint32_t u32AlignMib)
    __attribute__((alias("gj_gpu_heap_budget_fits_aligned_6036")));

uint32_t __gj_gpu_heap_budget_remain_mib_6036(uint32_t u32FreeMib,
                                              uint32_t u32NeedMib)
    __attribute__((alias("gj_gpu_heap_budget_remain_mib_6036")));

uint32_t __gj_gpu_heap_budget_fit_ready_6036(void)
    __attribute__((alias("gj_gpu_heap_budget_fit_ready_6036")));
