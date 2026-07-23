/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2409: uint64_t saturating arithmetic right
 * shift (signed view of bits).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_ashr_sat(uint64_t x, unsigned n);
 *     - Arithmetic right shift of x treated as a two's-complement
 *       signed 64-bit value, result re-interpreted as uint64_t.
 *       For n in 0..63, shift in the sign bit. If n >= 64, return all
 *       zeros (non-negative view) or all ones (negative view). No
 *       shift UB; pure integer; no builtins; no signed-shift
 *       implementation-defined reliance.
 *   uint64_t __gj_u64_ashr_sat  (alias)
 *   __libcgj_batch2409_marker = "libcgj-batch2409"
 *
 * Milestone 2410 exclusive shift/rotate wave (2401-2410). Unique
 * gj_u64_ashr_sat surface only; no multi-def. Distinct from
 * gj_u64_shr_sat (batch2406 logical).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2409_marker[] = "libcgj-batch2409";

/* Sign bit of a 64-bit two's-complement view. */
#define B2409_SIGN  ((uint64_t)1 << 63)

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Arithmetic right shift of u64X as a signed view. n >= 64 saturates
 * to 0 or all-ones by sign. Implemented with unsigned ops only so the
 * result is fully defined (no signed >> reliance).
 */
static uint64_t
b2409_ashr_sat(uint64_t u64X, unsigned uN)
{
	if (uN >= 64u) {
		if ((u64X & B2409_SIGN) != 0u) {
			return ~(uint64_t)0;
		}
		return 0u;
	}
	if (uN == 0u) {
		return u64X;
	}
	if ((u64X & B2409_SIGN) == 0u) {
		return u64X >> uN;
	}
	/* Negative view: logical shift then fill high bits with ones. */
	return (u64X >> uN) | (~(uint64_t)0 << (64u - uN));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_ashr_sat - saturating arithmetic right shift (signed view).
 *
 * x: bit pattern to shift (MSB is the sign bit)
 * n: shift count; n >= 64 -> 0 if sign clear, all ones if sign set
 *
 * Returns the arithmetic-right-shifted bit pattern as uint64_t.
 * Does not call libc.
 */
uint64_t
gj_u64_ashr_sat(uint64_t u64X, unsigned uN)
{
	(void)NULL;
	return b2409_ashr_sat(u64X, uN);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_ashr_sat(uint64_t u64X, unsigned uN)
    __attribute__((alias("gj_u64_ashr_sat")));
