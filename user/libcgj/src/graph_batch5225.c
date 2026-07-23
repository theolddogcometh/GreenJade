/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5225: DMA scatter-gather segment byte offset.
 *
 * Surface (unique symbols):
 *   uint64_t gj_dma_sg_offset_u(uint64_t i, uint64_t max_seg);
 *     - Byte offset of SG entry i when segments are max_seg bytes each:
 *       i * max_seg. On multiplication overflow returns 0.
 *   uint64_t __gj_dma_sg_offset_u  (alias)
 *   __libcgj_batch5225_marker = "libcgj-batch5225"
 *
 * Exclusive continuum CREATE-ONLY (5221-5230: dma_align_u,
 * dma_is_aligned_u, dma_mask_ok_u, dma_sg_count_u, dma_sg_offset_u,
 * dma_coherent_u, dma_dir_to_dev_u, dma_dir_from_dev_u, dma_dir_bidir_u,
 * batch_id_5230). Unique gj_dma_sg_offset_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5225_marker[] = "libcgj-batch5225";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b5225_sg_offset(uint64_t u64I, uint64_t u64MaxSeg)
{
	/* i * max_seg with overflow → 0 (safe sentinel for freestanding). */
	if (u64I != 0ULL && u64MaxSeg > (UINT64_MAX / u64I)) {
		return 0ULL;
	}
	return u64I * u64MaxSeg;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dma_sg_offset_u - byte offset of the i-th max-sized SG segment.
 *
 * i:       zero-based segment index
 * max_seg: maximum bytes per SG entry
 *
 * Returns i * max_seg, or 0 on multiplication overflow (also when either
 * operand is 0). Self-contained; no parent wires.
 */
uint64_t
gj_dma_sg_offset_u(uint64_t u64I, uint64_t u64MaxSeg)
{
	(void)NULL;
	return b5225_sg_offset(u64I, u64MaxSeg);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_dma_sg_offset_u(uint64_t u64I, uint64_t u64MaxSeg)
    __attribute__((alias("gj_dma_sg_offset_u")));
