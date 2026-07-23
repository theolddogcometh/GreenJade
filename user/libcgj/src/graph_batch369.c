/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch369: rotate-left a uint64_t by r bits.
 *
 * Surface (unique symbols):
 *   uint64_t gj_rotl64(uint64_t x, unsigned r);
 *     — Circular left rotate of x by (r mod 64) bit positions.
 *       r is reduced modulo 64 so any unsigned rotate count is defined
 *       (including 0, which returns x unchanged).
 *   uint64_t __gj_rotl64  (alias)
 *   __libcgj_batch369_marker = "libcgj-batch369"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch369_marker[] = "libcgj-batch369";

/*
 * gj_rotl64 — left-rotate a 64-bit word.
 *
 * x: value to rotate
 * r: rotate count (mod 64)
 *
 * Zero-count branch avoids a C undefined 64-bit shift of 64.
 */
uint64_t
gj_rotl64(uint64_t u64X, unsigned uR)
{
	unsigned uAmt;

	uAmt = uR & 63u;
	if (uAmt == 0u) {
		return u64X;
	}
	return (u64X << uAmt) | (u64X >> (64u - uAmt));
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_rotl64(uint64_t u64X, unsigned uR)
    __attribute__((alias("gj_rotl64")));
