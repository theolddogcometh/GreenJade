/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6124: Mesa RADV feature-bit clear.
 *
 * Surface (unique symbols):
 *   uint32_t gj_radv_feat_bit_clear_6124(uint32_t mask, uint32_t bit);
 *     - Return mask with bit cleared when bit < 32; else return mask
 *       unchanged (out-of-range is a no-op).
 *   uint32_t __gj_radv_feat_bit_clear_6124  (alias)
 *   __libcgj_batch6124_marker = "libcgj-batch6124"
 *
 * Exclusive CREATE-ONLY (6121-6130: mesa radv feature bit stubs).
 * Unique gj_radv_feat_bit_clear_6124 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6124_marker[] = "libcgj-batch6124";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6124_clear(uint32_t u32Mask, uint32_t u32Bit)
{
	if (u32Bit >= 32u) {
		return u32Mask;
	}
	return u32Mask & ~(1u << u32Bit);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_radv_feat_bit_clear_6124 - clear one bit in a RADV feature mask.
 *
 * mask: soft RADV feature bitmask
 * bit:  bit index (0 = LSB); bit >= 32 leaves mask unchanged
 *
 * Returns updated mask. Soft pure-data op; no Vulkan ICD.
 */
uint32_t
gj_radv_feat_bit_clear_6124(uint32_t u32Mask, uint32_t u32Bit)
{
	(void)NULL;
	return b6124_clear(u32Mask, u32Bit);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_radv_feat_bit_clear_6124(uint32_t u32Mask, uint32_t u32Bit)
    __attribute__((alias("gj_radv_feat_bit_clear_6124")));
