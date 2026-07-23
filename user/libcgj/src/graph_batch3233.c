/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3233: deposit a bitfield into a uint64_t (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_deposit_bits_u(uint64_t x, unsigned lo, unsigned len,
 *                                  uint64_t v);
 *     - Replace the len bits of x starting at bit lo (LSB = bit 0) with
 *       the low len bits of v. Bits of x outside [lo, lo+len) are
 *       preserved. len == 0 leaves x unchanged. If the field would
 *       extend past bit 63, only the bits that fit (64 - lo) are written.
 *       lo >= 64 is a no-op (returns x).
 *   uint64_t __gj_u64_deposit_bits_u  (alias)
 *   __libcgj_batch3233_marker = "libcgj-batch3233"
 *
 * CREATE-ONLY exclusive bitops wave (3231-3240). Unique
 * gj_u64_deposit_bits_u surface only; no multi-def. Distinct from
 * gj_deposit_bits_u64 (batch594) and gj_u32_deposit_bits_u (batch3232).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3233_marker[] = "libcgj-batch3233";

/* ---- freestanding helpers ---------------------------------------------- */

/* Deposit low uLen bits of u64V into u64X at bit uLo. */
static uint64_t
b3233_deposit(uint64_t u64X, unsigned uLo, unsigned uLen, uint64_t u64V)
{
	unsigned uW;
	uint64_t u64Mask;

	if (uLen == 0u || uLo >= 64u) {
		return u64X;
	}

	uW = uLen;
	if (uW > 64u - uLo) {
		uW = 64u - uLo;
	}

	if (uW == 64u) {
		return u64V;
	}

	u64Mask = (UINT64_C(1) << uW) - UINT64_C(1);
	return (u64X & ~(u64Mask << uLo)) | ((u64V & u64Mask) << uLo);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_deposit_bits_u - bitfield insert into a 64-bit word.
 *
 * x:   host word
 * lo:  first bit index to replace (0 = LSB)
 * len: number of bits to replace
 * v:   source; only its low len bits (after clipping) are deposited
 *
 * Does not call libc. No parent wires.
 */
uint64_t
gj_u64_deposit_bits_u(uint64_t u64X, unsigned uLo, unsigned uLen,
    uint64_t u64V)
{
	(void)NULL;
	return b3233_deposit(u64X, uLo, uLen, u64V);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_deposit_bits_u(uint64_t u64X, unsigned uLo, unsigned uLen,
    uint64_t u64V)
    __attribute__((alias("gj_u64_deposit_bits_u")));
