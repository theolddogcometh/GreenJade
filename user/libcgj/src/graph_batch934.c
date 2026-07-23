/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch934: extract a bitfield from a uint32_t.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bitfield_get_u32(uint32_t x, unsigned lo, unsigned len);
 *     — Return the len bits of x starting at bit lo (LSB = 0), right-aligned.
 *       len == 0 or lo >= 32 → 0. Field clipped to bits that fit in 32.
 *   uint32_t __gj_bitfield_get_u32  (alias)
 *   __libcgj_batch934_marker = "libcgj-batch934"

 *
 * Does NOT define gj_deposit_bits_u64 (batch594) — 32-bit get only.
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch934_marker[] = "libcgj-batch934";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitfield_get_u32 — extract a contiguous bitfield from a 32-bit word.
 *
 * u32X:  host word
 * uLo:   first bit index (0 = LSB)
 * uLen:  number of bits to extract
 *
 * Returns the field shifted down so its LSB is at bit 0 of the result.
 * Avoids shift-by-32 UB by clipping len to 32 - lo.
 */
uint32_t
gj_bitfield_get_u32(uint32_t u32X, unsigned uLo, unsigned uLen)
{
	unsigned uW;
	uint32_t u32Mask;

	if (uLen == 0u || uLo >= 32u) {
		return 0u;
	}

	uW = uLen;
	if (uW > 32u - uLo) {
		uW = 32u - uLo;
	}

	if (uW == 32u) {
		return u32X;
	}

	u32Mask = (((uint32_t)1u) << uW) - 1u;
	return (u32X >> uLo) & u32Mask;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bitfield_get_u32(uint32_t u32X, unsigned uLo, unsigned uLen)
    __attribute__((alias("gj_bitfield_get_u32")));
