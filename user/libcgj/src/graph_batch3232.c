/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3232: deposit a bitfield into a uint32_t (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_deposit_bits_u(uint32_t x, unsigned lo, unsigned len,
 *                                  uint32_t v);
 *     - Replace the len bits of x starting at bit lo (LSB = bit 0) with
 *       the low len bits of v. Bits of x outside [lo, lo+len) are
 *       preserved. len == 0 leaves x unchanged. If the field would
 *       extend past bit 31, only the bits that fit (32 - lo) are written.
 *       lo >= 32 is a no-op (returns x).
 *   uint32_t __gj_u32_deposit_bits_u  (alias)
 *   __libcgj_batch3232_marker = "libcgj-batch3232"
 *
 * CREATE-ONLY exclusive bitops wave (3231-3240). Unique
 * gj_u32_deposit_bits_u surface only; no multi-def. Distinct from
 * gj_bitfield_set_u32 (batch935) and gj_deposit_bits_u64 (batch594).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3232_marker[] = "libcgj-batch3232";

/* ---- freestanding helpers ---------------------------------------------- */

/* Deposit low uLen bits of u32V into u32X at bit uLo. */
static uint32_t
b3232_deposit(uint32_t u32X, unsigned uLo, unsigned uLen, uint32_t u32V)
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

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_deposit_bits_u - bitfield insert into a 32-bit word.
 *
 * x:   host word
 * lo:  first bit index to replace (0 = LSB)
 * len: number of bits to replace
 * v:   source; only its low len bits (after clipping) are deposited
 *
 * Does not call libc. No parent wires.
 */
uint32_t
gj_u32_deposit_bits_u(uint32_t u32X, unsigned uLo, unsigned uLen,
    uint32_t u32V)
{
	(void)NULL;
	return b3232_deposit(u32X, uLo, uLen, u32V);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_deposit_bits_u(uint32_t u32X, unsigned uLo, unsigned uLen,
    uint32_t u32V)
    __attribute__((alias("gj_u32_deposit_bits_u")));
