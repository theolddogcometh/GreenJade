/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6122: Mesa RADV feature-bit test.
 *
 * Surface (unique symbols):
 *   uint32_t gj_radv_feat_bit_test_6122(uint32_t mask, uint32_t bit);
 *     - Return 1 if bit < 32 and (mask & (1u << bit)) != 0; else 0.
 *   uint32_t __gj_radv_feat_bit_test_6122  (alias)
 *   __libcgj_batch6122_marker = "libcgj-batch6122"
 *
 * Exclusive CREATE-ONLY (6121-6130: mesa radv feature bit stubs).
 * Unique gj_radv_feat_bit_test_6122 surface only; no multi-def.
 * Distinct from gj_feature_test_bit (batch1185). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6122_marker[] = "libcgj-batch6122";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6122_test(uint32_t u32Mask, uint32_t u32Bit)
{
	if (u32Bit >= 32u) {
		return 0u;
	}
	return ((u32Mask >> u32Bit) & 1u) != 0u ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_radv_feat_bit_test_6122 - test one bit in a RADV feature mask.
 *
 * mask: soft RADV feature bitmask
 * bit:  bit index (0 = LSB); bit >= 32 yields 0 (no shift UB)
 *
 * Returns 1 if set, else 0. Soft pure-data bit test; no Vulkan ICD.
 */
uint32_t
gj_radv_feat_bit_test_6122(uint32_t u32Mask, uint32_t u32Bit)
{
	(void)NULL;
	return b6122_test(u32Mask, u32Bit);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_radv_feat_bit_test_6122(uint32_t u32Mask, uint32_t u32Bit)
    __attribute__((alias("gj_radv_feat_bit_test_6122")));
