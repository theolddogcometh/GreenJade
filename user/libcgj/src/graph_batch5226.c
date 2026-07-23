/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5226: DMA coherent-capable capability tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_dma_coherent_u(void);
 *     - Returns 1 (platform reports coherent DMA as available for the
 *       freestanding continuum product tag; not a runtime probe).
 *   uint32_t __gj_dma_coherent_u  (alias)
 *   __libcgj_batch5226_marker = "libcgj-batch5226"
 *
 * Exclusive continuum CREATE-ONLY (5221-5230: dma_align_u,
 * dma_is_aligned_u, dma_mask_ok_u, dma_sg_count_u, dma_sg_offset_u,
 * dma_coherent_u, dma_dir_to_dev_u, dma_dir_from_dev_u, dma_dir_bidir_u,
 * batch_id_5230). Unique gj_dma_coherent_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5226_marker[] = "libcgj-batch5226";

/* Coherent DMA capability tag for this continuum. */
#define B5226_COHERENT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5226_coherent(void)
{
	return B5226_COHERENT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dma_coherent_u - report coherent DMA capability tag.
 *
 * Always returns 1. Soft compile-time product tag for IOMMU/DMA wave
 * 5221-5230; does not probe hardware. No parent wires.
 */
uint32_t
gj_dma_coherent_u(void)
{
	(void)NULL;
	return b5226_coherent();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dma_coherent_u(void)
    __attribute__((alias("gj_dma_coherent_u")));
