/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3258: uint64 ratio as unsigned Q16.16 (_u)
 * without __int128.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_ratio_q16_u(uint64_t a, uint64_t b);
 *     - Return floor((a * 65536) / b) as an unsigned Q16.16-scale
 *       fixed-point ratio of a/b (1.0 == 65536). b==0 → 0.
 *       Uses a shift fast path when a fits in 48 bits; otherwise
 *       decomposes (a/b)<<16 + ((a%b)<<16)/b without forming a
 *       wrapping product. No __int128.
 *   uint64_t __gj_u64_ratio_q16_u  (alias)
 *   __libcgj_batch3258_marker = "libcgj-batch3258"
 *
 * CREATE-ONLY exclusive continuum wave (3251-3260). Unique
 * gj_u64_ratio_q16_u surface only; no multi-def. Distinct from
 * gj_ratio_u64 (batch998) and gj_u64_mul_div_u (batch3052).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3258_marker[] = "libcgj-batch3258";

/* ---- freestanding helpers (no __int128) -------------------------------- */

/*
 * floor((a * 65536) / b). Fast path when a << 16 fits in 64 bits;
 * otherwise split a = q*b + r and combine (q << 16) + ((r << 16) / b).
 * When r is large enough that r << 16 would wrap, use a 16-step
 * long-division of the (0:r<<16) value by b via successive double.
 */
static uint64_t
b3258_ratio_q16(uint64_t u64A, uint64_t u64B)
{
	uint64_t u64Q;
	uint64_t u64R;
	uint64_t u64Frac;
	uint32_t uBit;

	if (u64B == 0ull) {
		return 0ull;
	}
	if (u64A == 0ull) {
		return 0ull;
	}

	/* Fast path: a * 65536 fits in 64 bits. */
	if (u64A <= (UINT64_MAX >> 16)) {
		return (u64A << 16) / u64B;
	}

	/*
	 * a = q*b + r with r < b. Want (q*b + r) * 65536 / b
	 *   = q * 65536 + (r * 65536) / b.
	 * Guard q << 16 overflow: if q > UINT64_MAX >> 16, saturate.
	 */
	u64Q = u64A / u64B;
	u64R = u64A % u64B;

	if (u64Q > (UINT64_MAX >> 16)) {
		return UINT64_MAX;
	}

	/* (r * 65536) / b — r < b; may still need care for large r. */
	if (u64R <= (UINT64_MAX >> 16)) {
		u64Frac = (u64R << 16) / u64B;
	} else {
		/*
		 * Form (0 : r) then left-shift 16 into a 128-bit
		 * intermediate via binary long division by b for 16 bits
		 * of quotient after first placing r in the high word.
		 * Equivalent: start R=r, for 16 steps: R = (R<<1); if
		 * R>=b then R-=b and set quotient bit. But r may have
		 * MSB set so R<<1 can lose the carry-out — track it.
		 */
		uint64_t u64Rem = u64R;
		uint64_t u64Carry;

		u64Frac = 0ull;
		for (uBit = 0u; uBit < 16u; uBit++) {
			u64Carry = u64Rem >> 63;
			u64Rem <<= 1;
			u64Frac <<= 1;
			if (u64Carry != 0ull || u64Rem >= u64B) {
				u64Rem -= u64B;
				u64Frac |= 1ull;
			}
		}
	}

	return (u64Q << 16) + u64Frac;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_ratio_q16_u - unsigned Q16-scale ratio floor((a * 65536) / b).
 *
 * a: numerator
 * b: denominator (b==0 yields 0)
 *
 * Portable path without __int128. Quotients that would exceed
 * UINT64_MAX saturate to UINT64_MAX. Self-contained; no parent wires.
 */
uint64_t
gj_u64_ratio_q16_u(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b3258_ratio_q16(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_ratio_q16_u(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_ratio_q16_u")));
