/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1515: freestanding 32-bit rotate left (n form).
 *
 * Surface (unique symbols):
 *   uint32_t gj_rotl32_n(uint32_t x, unsigned n);
 *     — Circular left rotate of x by (n mod 32) bit positions.
 *       n reduced with n &= 31; 0 returns x unchanged.
 *   uint32_t __gj_rotl32_n  (alias)
 *   __libcgj_batch1515_marker = "libcgj-batch1515"
 *
 * Distinct from gj_rotl32 (batch771). Unique _n surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied. No real
 * crypto copied.
 */

#include <stdint.h>

const char __libcgj_batch1515_marker[] = "libcgj-batch1515";

/*
 * gj_rotl32_n — left-rotate a 32-bit word by n bits (mod 32).
 *
 * x: value to rotate
 * n: rotate count (reduced mod 32; 0 → identity)
 *
 * Avoids undefined shift amounts by masking n and special-casing 0.
 */
uint32_t
gj_rotl32_n(uint32_t u32X, unsigned uN)
{
	uN &= 31u;
	if (uN == 0u) {
		return u32X;
	}
	return (u32X << uN) | (u32X >> (32u - uN));
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rotl32_n(uint32_t u32X, unsigned uN)
    __attribute__((alias("gj_rotl32_n")));
