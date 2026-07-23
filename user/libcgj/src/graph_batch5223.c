/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5223: DMA bus-mask fitness test (IOMMU/DMA).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dma_mask_ok_u(uint64_t addr, uint64_t mask);
 *     - Return 1 when (addr & ~mask) == 0 (address fits the device bus
 *       address mask), else 0. Classic DMA_BIT_MASK(n) fitness check.
 *   uint32_t __gj_dma_mask_ok_u  (alias)
 *   __libcgj_batch5223_marker = "libcgj-batch5223"
 *
 * Exclusive continuum CREATE-ONLY (5221-5230: dma_align_u,
 * dma_is_aligned_u, dma_mask_ok_u, dma_sg_count_u, dma_sg_offset_u,
 * dma_coherent_u, dma_dir_to_dev_u, dma_dir_from_dev_u, dma_dir_bidir_u,
 * batch_id_5230). Unique gj_dma_mask_ok_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5223_marker[] = "libcgj-batch5223";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5223_mask_ok(uint64_t u64Addr, uint64_t u64Mask)
{
	/* Bits of addr outside the permitted bus mask must be clear. */
	if ((u64Addr & ~u64Mask) == 0ULL) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dma_mask_ok_u - test whether addr is legal under a DMA bus mask.
 *
 * addr: candidate DMA / bus address
 * mask: permitted address bits (e.g. 0xffffffff for 32-bit DMA)
 *
 * Returns 1 when (addr & ~mask) == 0, else 0. Self-contained; no
 * parent wires.
 */
uint32_t
gj_dma_mask_ok_u(uint64_t u64Addr, uint64_t u64Mask)
{
	(void)NULL;
	return b5223_mask_ok(u64Addr, u64Mask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dma_mask_ok_u(uint64_t u64Addr, uint64_t u64Mask)
    __attribute__((alias("gj_dma_mask_ok_u")));
