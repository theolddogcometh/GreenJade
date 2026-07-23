/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch566: rotate-right a uint64_t by n bits.
 *
 * Surface (unique symbols):
 *   uint64_t gj_rotr_u64(uint64_t x, unsigned n);
 *     — Circular right rotate of x by (n mod 64) bit positions.
 *       n is reduced with n &= 63 so any unsigned rotate count is defined
 *       (including 0, which returns x unchanged).
 *   uint64_t __gj_rotr_u64  (alias)
 *   __libcgj_batch566_marker = "libcgj-batch566"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch566_marker[] = "libcgj-batch566";

/*
 * gj_rotr_u64 — right-rotate a 64-bit word.
 *
 * x: value to rotate
 * n: rotate count (mod 64 via n &= 63)
 *
 * Zero-count branch avoids a C undefined 64-bit shift of 64.
 */
uint64_t
gj_rotr_u64(uint64_t x, unsigned n)
{
	n &= 63u;
	if (n == 0u) {
		return x;
	}
	return (x >> n) | (x << (64u - n));
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_rotr_u64(uint64_t x, unsigned n)
    __attribute__((alias("gj_rotr_u64")));
