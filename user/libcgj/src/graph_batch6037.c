/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6037: GPU memory heap budget alignment stubs.
 *
 * Surface (unique symbols):
 *   uint32_t gj_gpu_heap_budget_align_mib_6037(void);
 *     - Returns 1 (soft default heap allocation align in MiB).
 *   uint32_t gj_gpu_heap_budget_page_align_kib_6037(void);
 *     - Returns 64 (soft 64 KiB GPU page/heap granule).
 *   uint32_t gj_gpu_heap_budget_align_up_mib_6037(uint32_t size_mib,
 *                                                uint32_t align_mib);
 *     - Round size_mib up to multiple of align_mib (power-of-two).
 *       Bad align → size unchanged; overflow → 0.
 *   uint32_t gj_gpu_heap_budget_align_ready_6037(void);
 *     - Returns 1 (soft align-path ready lamp).
 *   uint32_t __gj_gpu_heap_budget_align_mib_6037  (alias)
 *   uint32_t __gj_gpu_heap_budget_page_align_kib_6037  (alias)
 *   uint32_t __gj_gpu_heap_budget_align_up_mib_6037  (alias)
 *   uint32_t __gj_gpu_heap_budget_align_ready_6037  (alias)
 *   __libcgj_batch6037_marker = "libcgj-batch6037"
 *
 * Exclusive continuum CREATE-ONLY (6031-6040: GPU memory heap budget
 * stubs). Alignment helpers; distinct from gj_vk_align_up (1851). No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6037_marker[] = "libcgj-batch6037";

#define B6037_ALIGN_MIB       1u
#define B6037_PAGE_ALIGN_KIB  64u
#define B6037_READY           1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6037_align_up(uint32_t u32Size, uint32_t u32Align)
{
	uint32_t u32Mask;

	if (u32Align == 0u || (u32Align & (u32Align - 1u)) != 0u) {
		return u32Size;
	}
	u32Mask = u32Align - 1u;
	if (u32Size > (UINT32_MAX - u32Mask)) {
		return 0u;
	}
	return (u32Size + u32Mask) & ~u32Mask;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gpu_heap_budget_align_mib_6037 - soft default heap align in MiB.
 *
 * Always returns 1.
 */
uint32_t
gj_gpu_heap_budget_align_mib_6037(void)
{
	(void)NULL;
	return B6037_ALIGN_MIB;
}

/*
 * gj_gpu_heap_budget_page_align_kib_6037 - soft GPU page granule KiB.
 *
 * Always returns 64.
 */
uint32_t
gj_gpu_heap_budget_page_align_kib_6037(void)
{
	return B6037_PAGE_ALIGN_KIB;
}

/*
 * gj_gpu_heap_budget_align_up_mib_6037 - soft power-of-two MiB roundup.
 *
 * size_mib:  size to align
 * align_mib: power-of-two alignment (0 / non-pot → identity)
 *
 * Returns aligned size, identity on bad align, 0 on overflow.
 */
uint32_t
gj_gpu_heap_budget_align_up_mib_6037(uint32_t u32SizeMib, uint32_t u32AlignMib)
{
	return b6037_align_up(u32SizeMib, u32AlignMib);
}

/*
 * gj_gpu_heap_budget_align_ready_6037 - soft align-path ready lamp.
 *
 * Always returns 1.
 */
uint32_t
gj_gpu_heap_budget_align_ready_6037(void)
{
	return B6037_READY;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_gpu_heap_budget_align_mib_6037(void)
    __attribute__((alias("gj_gpu_heap_budget_align_mib_6037")));

uint32_t __gj_gpu_heap_budget_page_align_kib_6037(void)
    __attribute__((alias("gj_gpu_heap_budget_page_align_kib_6037")));

uint32_t __gj_gpu_heap_budget_align_up_mib_6037(uint32_t u32SizeMib,
                                               uint32_t u32AlignMib)
    __attribute__((alias("gj_gpu_heap_budget_align_up_mib_6037")));

uint32_t __gj_gpu_heap_budget_align_ready_6037(void)
    __attribute__((alias("gj_gpu_heap_budget_align_ready_6037")));
