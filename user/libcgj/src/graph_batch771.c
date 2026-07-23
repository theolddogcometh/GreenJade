/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch771: freestanding 32-bit rotate left.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rotl32(uint32_t x, unsigned n);
 *     — Rotate x left by (n & 31) bits. n == 0 (mod 32) returns x.
 *   uint32_t __gj_rotl32  (alias)
 *   __libcgj_batch771_marker = "libcgj-batch771"
 *
 * Distinct from file-static bNN_rotl32 helpers elsewhere — unique
 * gj_rotl32 public surface only; no multi-def with gj_rotr32 (batch772).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch771_marker[] = "libcgj-batch771";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rotl32 — left-rotate a 32-bit word by n bits (mod 32).
 *
 * x: value to rotate
 * n: rotate count (reduced mod 32; 0 → identity)
 *
 * Avoids undefined shift amounts by masking n and special-casing 0.
 */
uint32_t
gj_rotl32(uint32_t u32X, unsigned uN)
{
	uN &= 31u;
	if (uN == 0u) {
		return u32X;
	}
	return (u32X << uN) | (u32X >> (32u - uN));
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rotl32(uint32_t u32X, unsigned uN)
    __attribute__((alias("gj_rotl32")));
