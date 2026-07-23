/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3421: software parallel-bits extract (u32).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_pext_soft_u(uint32_t src, uint32_t mask);
 *     - Soft PEXT: gather bits of src selected by mask into the low
 *       contiguous bit positions of the result (LSB-first pack).
 *       Pure integer walk; no BMI2 / builtins.
 *   uint32_t __gj_u32_pext_soft_u  (alias)
 *   __libcgj_batch3421_marker = "libcgj-batch3421"
 *
 * Bitops exclusive CREATE-ONLY wave (3421-3430). Unique
 * gj_u32_pext_soft_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3421_marker[] = "libcgj-batch3421";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Soft PEXT for 32-bit words: for each set bit of mask (LSB to MSB),
 * append the corresponding src bit into the next low result position.
 */
static uint32_t
b3421_pext32(uint32_t u32Src, uint32_t u32Mask)
{
	uint32_t u32Res;
	uint32_t u32Bit;
	uint32_t u32Lsb;

	u32Res = 0u;
	u32Bit = 1u;

	while (u32Mask != 0u) {
		u32Lsb = u32Mask & (uint32_t)(-(int32_t)u32Mask);
		if ((u32Src & u32Lsb) != 0u) {
			u32Res |= u32Bit;
		}
		u32Mask ^= u32Lsb;
		u32Bit <<= 1;
	}

	return u32Res;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_pext_soft_u - software parallel bits extract (32-bit).
 *
 * src:  source word
 * mask: selector; 1-bits mark src positions to gather (LSB-first pack)
 *
 * Returns packed extracted bits in the low positions. mask == 0 -> 0.
 * Does not call libc. No parent wires.
 */
uint32_t
gj_u32_pext_soft_u(uint32_t u32Src, uint32_t u32Mask)
{
	(void)NULL;
	return b3421_pext32(u32Src, u32Mask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_pext_soft_u(uint32_t u32Src, uint32_t u32Mask)
    __attribute__((alias("gj_u32_pext_soft_u")));
