/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3422: software parallel-bits extract (u64).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_pext_soft_u(uint64_t src, uint64_t mask);
 *     - Soft PEXT: gather bits of src selected by mask into the low
 *       contiguous bit positions of the result (LSB-first pack).
 *       Pure integer walk; no BMI2 / builtins.
 *   uint64_t __gj_u64_pext_soft_u  (alias)
 *   __libcgj_batch3422_marker = "libcgj-batch3422"
 *
 * Bitops exclusive CREATE-ONLY wave (3421-3430). Unique
 * gj_u64_pext_soft_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3422_marker[] = "libcgj-batch3422";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Soft PEXT for 64-bit words: for each set bit of mask (LSB to MSB),
 * append the corresponding src bit into the next low result position.
 */
static uint64_t
b3422_pext64(uint64_t u64Src, uint64_t u64Mask)
{
	uint64_t u64Res;
	uint64_t u64Bit;
	uint64_t u64Lsb;

	u64Res = 0ull;
	u64Bit = 1ull;

	while (u64Mask != 0ull) {
		u64Lsb = u64Mask & (uint64_t)(-(int64_t)u64Mask);
		if ((u64Src & u64Lsb) != 0ull) {
			u64Res |= u64Bit;
		}
		u64Mask ^= u64Lsb;
		u64Bit <<= 1;
	}

	return u64Res;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_pext_soft_u - software parallel bits extract (64-bit).
 *
 * src:  source word
 * mask: selector; 1-bits mark src positions to gather (LSB-first pack)
 *
 * Returns packed extracted bits in the low positions. mask == 0 -> 0.
 * Does not call libc. No parent wires.
 */
uint64_t
gj_u64_pext_soft_u(uint64_t u64Src, uint64_t u64Mask)
{
	(void)NULL;
	return b3422_pext64(u64Src, u64Mask);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_pext_soft_u(uint64_t u64Src, uint64_t u64Mask)
    __attribute__((alias("gj_u64_pext_soft_u")));
