/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1016: 16-bit Galois LFSR step.
 *
 * Surface (unique symbols):
 *   uint16_t gj_lfsr16_next(uint16_t s);
 *     — Advance a 16-bit Galois LFSR one step and return the new state.
 *       Polynomial (right-shift Galois): 0xB400
 *       (x^16 + x^14 + x^13 + x^11 + 1). s == 0 is replaced with 1 so
 *       the register does not stick at the all-zero fixed point.
 *   uint16_t __gj_lfsr16_next  (alias)
 *   __libcgj_batch1016_marker = "libcgj-batch1016"
 *
 * Not cryptographically secure. Clean-room freestanding pure C (integer
 * only). Compiles with -ffreestanding -msse2 -Wall -Wextra -Werror. No
 * malloc, no errno, no libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1016_marker[] = "libcgj-batch1016";

/* Galois LFSR16 feedback polynomial (bits 15,13,12,10 after right shift). */
#define B1016_POLY  0xB400u

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_lfsr16_next — one Galois LFSR16 step from state s.
 *
 * s: current 16-bit state (0 is rewritten to 1 before stepping)
 *
 * Algorithm: lsb = s & 1; s >>= 1; if lsb then s ^= 0xB400. Returns the
 * updated state. Period is 2^16 - 1 for any non-zero start.
 */
uint16_t
gj_lfsr16_next(uint16_t s)
{
	uint16_t uLsb;

	if (s == 0u) {
		s = 1u;
	}

	uLsb = (uint16_t)(s & 1u);
	s = (uint16_t)(s >> 1);
	if (uLsb != 0u) {
		s = (uint16_t)(s ^ (uint16_t)B1016_POLY);
	}
	return s;
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_lfsr16_next(uint16_t s)
    __attribute__((alias("gj_lfsr16_next")));
