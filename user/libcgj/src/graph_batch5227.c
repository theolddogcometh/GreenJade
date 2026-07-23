/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5227: DMA direction constant TO_DEVICE.
 *
 * Surface (unique symbols):
 *   uint32_t gj_dma_dir_to_dev_u(void);
 *     - Returns 1 (DMA_TO_DEVICE direction code for freestanding DMA
 *       direction tagging; host → device).
 *   uint32_t __gj_dma_dir_to_dev_u  (alias)
 *   __libcgj_batch5227_marker = "libcgj-batch5227"
 *
 * Exclusive continuum CREATE-ONLY (5221-5230: dma_align_u,
 * dma_is_aligned_u, dma_mask_ok_u, dma_sg_count_u, dma_sg_offset_u,
 * dma_coherent_u, dma_dir_to_dev_u, dma_dir_from_dev_u, dma_dir_bidir_u,
 * batch_id_5230). Unique gj_dma_dir_to_dev_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5227_marker[] = "libcgj-batch5227";

/* DMA direction: host → device. */
#define B5227_DIR_TO_DEV  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5227_dir_to_dev(void)
{
	return B5227_DIR_TO_DEV;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dma_dir_to_dev_u - DMA direction code for host-to-device transfers.
 *
 * Always returns 1 (TO_DEVICE). Soft continuum direction tag; does not
 * program hardware. No parent wires.
 */
uint32_t
gj_dma_dir_to_dev_u(void)
{
	(void)NULL;
	return b5227_dir_to_dev();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dma_dir_to_dev_u(void)
    __attribute__((alias("gj_dma_dir_to_dev_u")));
