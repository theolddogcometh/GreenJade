/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5229: DMA direction constant BIDIRECTIONAL.
 *
 * Surface (unique symbols):
 *   uint32_t gj_dma_dir_bidir_u(void);
 *     - Returns 3 (DMA_BIDIRECTIONAL direction code for freestanding
 *       DMA direction tagging; host ↔ device).
 *   uint32_t __gj_dma_dir_bidir_u  (alias)
 *   __libcgj_batch5229_marker = "libcgj-batch5229"
 *
 * Exclusive continuum CREATE-ONLY (5221-5230: dma_align_u,
 * dma_is_aligned_u, dma_mask_ok_u, dma_sg_count_u, dma_sg_offset_u,
 * dma_coherent_u, dma_dir_to_dev_u, dma_dir_from_dev_u, dma_dir_bidir_u,
 * batch_id_5230). Unique gj_dma_dir_bidir_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5229_marker[] = "libcgj-batch5229";

/* DMA direction: host ↔ device. */
#define B5229_DIR_BIDIR  3u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5229_dir_bidir(void)
{
	return B5229_DIR_BIDIR;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dma_dir_bidir_u - DMA direction code for bidirectional transfers.
 *
 * Always returns 3 (BIDIRECTIONAL). Soft continuum direction tag; does
 * not program hardware. No parent wires.
 */
uint32_t
gj_dma_dir_bidir_u(void)
{
	(void)NULL;
	return b5229_dir_bidir();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dma_dir_bidir_u(void)
    __attribute__((alias("gj_dma_dir_bidir_u")));
