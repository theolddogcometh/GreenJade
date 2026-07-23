/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5222: DMA address alignment predicate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_dma_is_aligned_u(uint64_t addr, uint64_t align);
 *     - Return 1 if addr is a multiple of align (DMA-ready), else 0.
 *       align <= 1 treats every address as aligned.
 *   uint32_t __gj_dma_is_aligned_u  (alias)
 *   __libcgj_batch5222_marker = "libcgj-batch5222"
 *
 * Exclusive continuum CREATE-ONLY (5221-5230: dma_align_u,
 * dma_is_aligned_u, dma_mask_ok_u, dma_sg_count_u, dma_sg_offset_u,
 * dma_coherent_u, dma_dir_to_dev_u, dma_dir_from_dev_u, dma_dir_bidir_u,
 * batch_id_5230). Unique gj_dma_is_aligned_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5222_marker[] = "libcgj-batch5222";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5222_is_aligned(uint64_t u64Addr, uint64_t u64Align)
{
	if (u64Align <= 1ULL) {
		return 1u;
	}

	/* Power-of-two fast path (typical IOMMU granule). */
	if ((u64Align & (u64Align - 1ULL)) == 0ULL) {
		if ((u64Addr & (u64Align - 1ULL)) == 0ULL) {
			return 1u;
		}
		return 0u;
	}

	if ((u64Addr % u64Align) == 0ULL) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dma_is_aligned_u - test whether a DMA address meets alignment.
 *
 * addr:  DMA / bus address under test
 * align: required alignment in bytes (0/1 → always true)
 *
 * Returns 1 when aligned, 0 otherwise. Self-contained; no parent wires.
 */
uint32_t
gj_dma_is_aligned_u(uint64_t u64Addr, uint64_t u64Align)
{
	(void)NULL;
	return b5222_is_aligned(u64Addr, u64Align);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dma_is_aligned_u(uint64_t u64Addr, uint64_t u64Align)
    __attribute__((alias("gj_dma_is_aligned_u")));
