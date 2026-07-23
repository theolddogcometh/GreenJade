/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2039: saturating uint64_t power (base^exp).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_pow_sat(uint64_t base, uint64_t exp);
 *     — base^exp with saturation at UINT64_MAX on overflow.
 *       exp is supported in 0..64 inclusive; exp > 64 → UINT64_MAX
 *       when base >= 2 (would overflow), base == 0 → 0, base == 1 → 1.
 *       0^0 is defined as 1. 1^exp == 1 for any exp. 0^exp == 0 for exp > 0.
 *   uint64_t __gj_u64_pow_sat  (alias)
 *   __libcgj_batch2039_marker = "libcgj-batch2039"
 *
 * Post-2030 u64 arith exclusive wave (2031–2040). Distinct from
 * gj_u64_pow10 (batch1735) — general base and sat semantics; unique
 * name; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2039_marker[] = "libcgj-batch2039";

/* ---- freestanding helpers ---------------------------------------------- */

/* Soft saturating mul; UINT64_MAX on overflow. */
static uint64_t
b2039_mul_sat(uint64_t u64A, uint64_t u64B)
{
	if (u64A == 0ull || u64B == 0ull) {
		return 0ull;
	}
	if (u64A > (UINT64_MAX / u64B)) {
		return UINT64_MAX;
	}
	return u64A * u64B;
}

/*
 * Binary exponentiation with soft saturation.
 * Caller: exp in 0..64 (or base in {0,1} for larger exp).
 */
static uint64_t
b2039_pow_sat(uint64_t u64Base, uint64_t u64Exp)
{
	uint64_t u64R = 1ull;
	uint64_t u64B = u64Base;
	uint64_t u64E = u64Exp;

	if (u64E == 0ull) {
		return 1ull;
	}
	if (u64B == 0ull) {
		return 0ull;
	}
	if (u64B == 1ull) {
		return 1ull;
	}

	/* exp > 64 with base >= 2 always overflows uint64. */
	if (u64E > 64ull) {
		return UINT64_MAX;
	}

	while (u64E > 0ull) {
		if ((u64E & 1ull) != 0ull) {
			u64R = b2039_mul_sat(u64R, u64B);
			if (u64R == UINT64_MAX) {
				return UINT64_MAX;
			}
		}
		u64E >>= 1;
		if (u64E == 0ull) {
			break;
		}
		u64B = b2039_mul_sat(u64B, u64B);
		if (u64B == UINT64_MAX) {
			/*
			 * Squared base saturated. Remaining multiplies of
			 * result by UINT64_MAX (or further sq) stay max
			 * when result already > 1, or if any odd bit left.
			 * Safer: if any bit remains in exp after this, sat.
			 */
			if (u64E != 0ull) {
				return UINT64_MAX;
			}
		}
	}
	return u64R;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_pow_sat — base^exp with saturation; exp intended in 0..64.
 *
 * base: power base
 * exp:  exponent (0 → 1; >64 with base>=2 → UINT64_MAX)
 */
uint64_t
gj_u64_pow_sat(uint64_t u64Base, uint64_t u64Exp)
{
	(void)NULL;
	return b2039_pow_sat(u64Base, u64Exp);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_pow_sat(uint64_t u64Base, uint64_t u64Exp)
    __attribute__((alias("gj_u64_pow_sat")));
