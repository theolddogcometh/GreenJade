/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1017: 32-bit Galois LFSR step.
 *
 * Surface (unique symbols):
 *   uint32_t gj_lfsr32_next(uint32_t s);
 *     — Advance a 32-bit Galois LFSR one step and return the new state.
 *       Polynomial (right-shift Galois): 0x80200003
 *       (x^32 + x^22 + x^2 + x + 1). s == 0 is replaced with 1 so the
 *       register does not stick at the all-zero fixed point.
 *   uint32_t __gj_lfsr32_next  (alias)
 *   __libcgj_batch1017_marker = "libcgj-batch1017"
 *
 * Distinct from gj_lfsr16_next (batch1016) — avoid multi-def.
 * Not cryptographically secure. Clean-room freestanding pure C (integer
 * only). Compiles with -ffreestanding -msse2 -Wall -Wextra -Werror. No
 * malloc, no errno, no libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1017_marker[] = "libcgj-batch1017";

/* Galois LFSR32 feedback polynomial. */
#define B1017_POLY  0x80200003u

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_lfsr32_next — one Galois LFSR32 step from state s.
 *
 * s: current 32-bit state (0 is rewritten to 1 before stepping)
 *
 * Algorithm: lsb = s & 1; s >>= 1; if lsb then s ^= 0x80200003.
 * Returns the updated state. Period is 2^32 - 1 for any non-zero start.
 */
uint32_t
gj_lfsr32_next(uint32_t s)
{
	uint32_t uLsb;

	if (s == 0u) {
		s = 1u;
	}

	uLsb = s & 1u;
	s >>= 1;
	if (uLsb != 0u) {
		s ^= B1017_POLY;
	}
	return s;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_lfsr32_next(uint32_t s)
    __attribute__((alias("gj_lfsr32_next")));
