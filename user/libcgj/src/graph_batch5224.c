/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5224: DMA scatter-gather segment count.
 *
 * Surface (unique symbols):
 *   uint64_t gj_dma_sg_count_u(uint64_t len, uint64_t max_seg);
 *     - Number of SG entries needed to map len bytes when each segment
 *       is at most max_seg bytes: ceil(len / max_seg).
 *       max_seg == 0 → 0. len == 0 → 0.
 *   uint64_t __gj_dma_sg_count_u  (alias)
 *   __libcgj_batch5224_marker = "libcgj-batch5224"
 *
 * Exclusive continuum CREATE-ONLY (5221-5230: dma_align_u,
 * dma_is_aligned_u, dma_mask_ok_u, dma_sg_count_u, dma_sg_offset_u,
 * dma_coherent_u, dma_dir_to_dev_u, dma_dir_from_dev_u, dma_dir_bidir_u,
 * batch_id_5230). Unique gj_dma_sg_count_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5224_marker[] = "libcgj-batch5224";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b5224_sg_count(uint64_t u64Len, uint64_t u64MaxSeg)
{
	if (u64MaxSeg == 0ULL || u64Len == 0ULL) {
		return 0ULL;
	}

	/* ceil(len / max_seg) without intermediate overflow of len+max_seg-1. */
	return ((u64Len - 1ULL) / u64MaxSeg) + 1ULL;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dma_sg_count_u - count scatter-gather segments for a length.
 *
 * len:     total transfer length in bytes
 * max_seg: maximum bytes per SG entry (0 → 0 segments)
 *
 * Returns ceil(len / max_seg), or 0 when len or max_seg is 0.
 * Self-contained; no parent wires.
 */
uint64_t
gj_dma_sg_count_u(uint64_t u64Len, uint64_t u64MaxSeg)
{
	(void)NULL;
	return b5224_sg_count(u64Len, u64MaxSeg);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_dma_sg_count_u(uint64_t u64Len, uint64_t u64MaxSeg)
    __attribute__((alias("gj_dma_sg_count_u")));
