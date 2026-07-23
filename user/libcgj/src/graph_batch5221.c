/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5221: DMA address align-up (IOMMU/DMA wave).
 *
 * Surface (unique symbols):
 *   uint64_t gj_dma_align_u(uint64_t addr, uint64_t align);
 *     - Round DMA bus address addr up to a multiple of align.
 *       align <= 1 is identity. Power-of-two uses mask form;
 *       otherwise general multiple. Overflow of the round-up → 0.
 *   uint64_t __gj_dma_align_u  (alias)
 *   __libcgj_batch5221_marker = "libcgj-batch5221"
 *
 * Exclusive continuum CREATE-ONLY (5221-5230: dma_align_u,
 * dma_is_aligned_u, dma_mask_ok_u, dma_sg_count_u, dma_sg_offset_u,
 * dma_coherent_u, dma_dir_to_dev_u, dma_dir_from_dev_u, dma_dir_bidir_u,
 * batch_id_5230). Distinct from gj_align_up_u64 (batch375),
 * gj_align_up (batch526), and gj_u32_align_up (batch779) — unique
 * gj_dma_align_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5221_marker[] = "libcgj-batch5221";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b5221_align(uint64_t u64Addr, uint64_t u64Align)
{
	uint64_t u64Mask;
	uint64_t u64Adj;
	uint64_t u64Rem;

	if (u64Align <= 1ULL) {
		return u64Addr;
	}

	/* Power-of-two DMA alignment (common for IOMMU page granules). */
	if ((u64Align & (u64Align - 1ULL)) == 0ULL) {
		u64Mask = u64Align - 1ULL;
		if (u64Addr > (UINT64_MAX - u64Mask)) {
			return 0ULL;
		}
		return (u64Addr + u64Mask) & ~u64Mask;
	}

	/* General multiple form. */
	u64Rem = u64Addr % u64Align;
	if (u64Rem == 0ULL) {
		return u64Addr;
	}
	u64Adj = u64Addr + (u64Align - u64Rem);
	if (u64Adj < u64Addr) {
		return 0ULL;
	}
	return u64Adj;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dma_align_u - round a DMA bus address up to alignment.
 *
 * addr:  candidate DMA / bus address
 * align: required alignment in bytes (0/1 → identity)
 *
 * Returns the smallest address >= addr that is a multiple of align,
 * or 0 on arithmetic overflow. Self-contained; no parent wires.
 */
uint64_t
gj_dma_align_u(uint64_t u64Addr, uint64_t u64Align)
{
	(void)NULL;
	return b5221_align(u64Addr, u64Align);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_dma_align_u(uint64_t u64Addr, uint64_t u64Align)
    __attribute__((alias("gj_dma_align_u")));
