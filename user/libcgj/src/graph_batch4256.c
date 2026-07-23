/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4256: soft parallel bit extract low (u32).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_bit_extract_low_u(uint32_t x, uint32_t mask);
 *     - Software PEXT-like: gather bits of x selected by mask into the
 *       low contiguous bit positions of the result (LSB-first pack).
 *       Pure integer walk; no BMI2 / builtins.
 *   uint32_t __gj_u32_bit_extract_low_u  (alias)
 *   __libcgj_batch4256_marker = "libcgj-batch4256"
 *
 * Exclusive continuum CREATE-ONLY (4251-4260). Unique
 * gj_u32_bit_extract_low_u surface only; no multi-def. Distinct from
 * gj_u32_pext_soft_u (batch3421) and gj_bit_extract_contiguous
 * (batch1013). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4256_marker[] = "libcgj-batch4256";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Soft PEXT loop: for each set bit of mask (LSB to MSB), append the
 * corresponding x bit into the next low result position.
 */
static uint32_t
b4256_extract(uint32_t u32X, uint32_t u32Mask)
{
	uint32_t u32Res;
	uint32_t u32Bit;
	uint32_t u32Lsb;

	u32Res = 0u;
	u32Bit = 1u;

	while (u32Mask != 0u) {
		u32Lsb = u32Mask & (uint32_t)(0u - u32Mask);
		if ((u32X & u32Lsb) != 0u) {
			u32Res |= u32Bit;
		}
		u32Mask ^= u32Lsb;
		u32Bit <<= 1;
	}

	return u32Res;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_bit_extract_low_u - software parallel bits extract (32-bit).
 *
 * x:    source word
 * mask: selector; 1-bits mark x positions to gather (LSB-first pack)
 *
 * Returns packed extracted bits in the low positions. mask == 0 -> 0.
 * Does not call libc. No parent wires.
 */
uint32_t
gj_u32_bit_extract_low_u(uint32_t u32X, uint32_t u32Mask)
{
	(void)NULL;
	return b4256_extract(u32X, u32Mask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_bit_extract_low_u(uint32_t u32X, uint32_t u32Mask)
    __attribute__((alias("gj_u32_bit_extract_low_u")));
