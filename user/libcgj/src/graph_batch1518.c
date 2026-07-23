/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1518: freestanding 64-bit rotate right (n form).
 *
 * Surface (unique symbols):
 *   uint64_t gj_rotr64_n(uint64_t x, unsigned n);
 *     — Circular right rotate of x by (n mod 64) bit positions.
 *       n reduced with n &= 63; 0 returns x unchanged.
 *   uint64_t __gj_rotr64_n  (alias)
 *   __libcgj_batch1518_marker = "libcgj-batch1518"
 *
 * Distinct from gj_rotr64 (batch370) and gj_rotr_u64 (batch566). Unique
 * _n surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied. No real
 * crypto copied.
 */

#include <stdint.h>

const char __libcgj_batch1518_marker[] = "libcgj-batch1518";

/*
 * gj_rotr64_n — right-rotate a 64-bit word by n bits (mod 64).
 *
 * x: value to rotate
 * n: rotate count (reduced mod 64; 0 → identity)
 *
 * Zero-count branch avoids a C undefined 64-bit shift of 64.
 */
uint64_t
gj_rotr64_n(uint64_t u64X, unsigned uN)
{
	uN &= 63u;
	if (uN == 0u) {
		return u64X;
	}
	return (u64X >> uN) | (u64X << (64u - uN));
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_rotr64_n(uint64_t u64X, unsigned uN)
    __attribute__((alias("gj_rotr64_n")));
