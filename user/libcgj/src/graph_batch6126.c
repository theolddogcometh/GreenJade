/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6126: Mesa RADV feature-mask popcount.
 *
 * Surface (unique symbols):
 *   uint32_t gj_radv_feat_mask_pop_6126(uint32_t mask);
 *     - Return number of set bits in mask (0..32). Soft pure-data
 *       popcount for a RADV feature word.
 *   uint32_t __gj_radv_feat_mask_pop_6126  (alias)
 *   __libcgj_batch6126_marker = "libcgj-batch6126"
 *
 * Exclusive CREATE-ONLY (6121-6130: mesa radv feature bit stubs).
 * Unique gj_radv_feat_mask_pop_6126 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6126_marker[] = "libcgj-batch6126";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6126_pop(uint32_t u32Mask)
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
 * gj_radv_feat_mask_pop_6126 - count set bits in a RADV feature mask.
 *
 * mask: soft RADV feature bitmask
 *
 * Returns 0..32. Soft pure-data popcount; no Vulkan ICD.
 */
uint32_t
gj_radv_feat_mask_pop_6126(uint32_t u32Mask)
{
	(void)NULL;
	return b6126_pop(u32Mask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_radv_feat_mask_pop_6126(uint32_t u32Mask)
    __attribute__((alias("gj_radv_feat_mask_pop_6126")));
