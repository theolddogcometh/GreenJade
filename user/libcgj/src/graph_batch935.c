/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch935: deposit a bitfield into a uint32_t.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bitfield_set_u32(uint32_t x, unsigned lo, unsigned len,
 *                                uint32_t v);
 *     — Replace the len bits of x starting at bit lo with the low len bits of v.
 *       len == 0 or lo >= 32 → return x. Field clipped to bits that fit.
 *   uint32_t __gj_bitfield_set_u32  (alias)
 *   __libcgj_batch935_marker = "libcgj-batch935"

 *
 * Does NOT define gj_deposit_bits_u64 (batch594) or gj_bitfield_get_u32
 * (batch934) — 32-bit set surface only.
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch935_marker[] = "libcgj-batch935";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitfield_set_u32 — insert a bitfield into a 32-bit word.
 *
 * u32X:  host word
 * uLo:   first bit index to replace (0 = LSB)
 * uLen:  number of bits to replace
 * u32V:  source; only its low len bits (after clipping) are deposited
 *
 * mask = low-len ones; clear those bits in x at lo, OR in (v & mask) << lo.
 */
uint32_t
gj_bitfield_set_u32(uint32_t u32X, unsigned uLo, unsigned uLen, uint32_t u32V)
{
	unsigned uW;
	uint32_t u32Mask;

	if (uLen == 0u || uLo >= 32u) {
		return u32X;
	}

	uW = uLen;
	if (uW > 32u - uLo) {
		uW = 32u - uLo;
	}

	if (uW == 32u) {
		return u32V;
	}

	u32Mask = (((uint32_t)1u) << uW) - 1u;
	return (u32X & ~(u32Mask << uLo)) | ((u32V & u32Mask) << uLo);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bitfield_set_u32(uint32_t u32X, unsigned uLo, unsigned uLen,
    uint32_t u32V)
    __attribute__((alias("gj_bitfield_set_u32")));
