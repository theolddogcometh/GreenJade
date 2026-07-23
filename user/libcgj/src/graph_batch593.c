/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch593: extract a bit-field from a uint64_t.
 *
 * Surface (unique symbols):
 *   uint64_t gj_extract_bits_u64(uint64_t x, unsigned lo, unsigned len);
 *     — Extract len consecutive bits starting at bit index lo (0-based
 *       from the LSB). Result is right-aligned in the low bits of the
 *       return value. len == 0 or lo >= 64 yields 0. If lo + len would
 *       run past bit 63, only the remaining high bits of x are taken
 *       (no undefined shift).
 *   __gj_extract_bits_u64  (alias)
 *   __libcgj_batch593_marker = "libcgj-batch593"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch593_marker[] = "libcgj-batch593";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_extract_bits_u64 — right-aligned bit-field extract.
 *
 * x:   source word
 * lo:  starting bit index (0 = LSB)
 * len: number of bits to extract
 *
 * Shift and mask carefully: never shift a uint64_t by 64, and never
 * form (1 << 64). Cap len against the remaining width above lo.
 */
uint64_t
gj_extract_bits_u64(uint64_t x, unsigned lo, unsigned len)
{
	uint64_t mask;
	unsigned rem;

	if (len == 0u || lo >= 64u) {
		return UINT64_C(0);
	}

	rem = 64u - lo;
	if (len > rem) {
		len = rem;
	}

	x >>= lo;
	if (len == 64u) {
		/* lo was 0 and full width requested — return whole word */
		return x;
	}

	mask = (UINT64_C(1) << len) - UINT64_C(1);
	return x & mask;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_extract_bits_u64(uint64_t x, unsigned lo, unsigned len)
    __attribute__((alias("gj_extract_bits_u64")));
