/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6556: VirtIO feature-mask popcount.
 *
 * Surface (unique symbols):
 *   uint32_t gj_virtio_feat_mask_pop_6556(uint32_t mask);
 *     - Return number of set bits in mask (0..32). Soft pure-data
 *       popcount for a VirtIO feature word.
 *   uint32_t __gj_virtio_feat_mask_pop_6556  (alias)
 *   __libcgj_batch6556_marker = "libcgj-batch6556"
 *
 * Exclusive continuum CREATE-ONLY (6551-6560: VirtIO feature bit stubs).
 * Unique gj_virtio_feat_mask_pop_6556 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6556_marker[] = "libcgj-batch6556";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6556_pop(uint32_t u32Mask)
{
	uint32_t u32N = 0u;
	uint32_t u32Bits = u32Mask;

	while (u32Bits != 0u) {
		u32N += (u32Bits & 1u);
		u32Bits >>= 1;
	}
	return u32N;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_virtio_feat_mask_pop_6556 - count set bits in a VirtIO feature mask.
 *
 * mask: soft VirtIO feature bitmask
 *
 * Returns 0..32. Soft pure-data popcount; no VirtIO device.
 */
uint32_t
gj_virtio_feat_mask_pop_6556(uint32_t u32Mask)
{
	(void)NULL;
	return b6556_pop(u32Mask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_virtio_feat_mask_pop_6556(uint32_t u32Mask)
    __attribute__((alias("gj_virtio_feat_mask_pop_6556")));
