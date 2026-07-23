/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1018: 32-bit xorshift step (Marsaglia).
 *
 * Surface (unique symbols):
 *   uint32_t gj_xorshift32_next(uint32_t s);
 *     — Advance a 32-bit xorshift state one step and return the new
 *       state. Triple (13, 17, 5): s ^= s << 13; s ^= s >> 17;
 *       s ^= s << 5. s == 0 is replaced with 1 so the generator does
 *       not stick at the all-zero fixed point.
 *   uint32_t __gj_xorshift32_next  (alias)
 *   __libcgj_batch1018_marker = "libcgj-batch1018"
 *
 * Distinct from gj_xorshift128_* (batch173) — avoid multi-def.
 * Not cryptographically secure. Clean-room freestanding pure C (integer
 * only). Compiles with -ffreestanding -msse2 -Wall -Wextra -Werror. No
 * malloc, no errno, no libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1018_marker[] = "libcgj-batch1018";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xorshift32_next — one xorshift32 step from state s.
 *
 * s: current 32-bit state (0 is rewritten to 1 before stepping)
 *
 * Classic Marsaglia xorshift32 with shifts (13, 17, 5). Returns the
 * updated state (also the sample). Period is 2^32 - 1 for non-zero start.
 */
uint32_t
gj_xorshift32_next(uint32_t s)
{
	if (s == 0u) {
		s = 1u;
	}

	s ^= s << 13;
	s ^= s >> 17;
	s ^= s << 5;
	return s;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_xorshift32_next(uint32_t s)
    __attribute__((alias("gj_xorshift32_next")));
