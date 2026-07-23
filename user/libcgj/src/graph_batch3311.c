/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3311: uint64_t integer power (_u surface).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_pow_u(uint64_t base, uint64_t exp);
 *     — base^exp via binary exponentiation.
 *       0^0 defined as 1. On any intermediate multiply overflow past
 *       UINT64_MAX, returns 0.
 *   uint64_t __gj_u64_pow_u  (alias)
 *   __libcgj_batch3311_marker = "libcgj-batch3311"
 *
 * CREATE-ONLY exclusive continuum wave (3311–3320). Unique gj_u64_pow_u
 * surface only; no multi-def. Distinct from gj_pow_u64 (batch385),
 * gj_u64_pow_sat (batch2039), and gj_u64_pow10 (batch1735). No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3311_marker[] = "libcgj-batch3311";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Multiply with overflow detection. Returns 0 and sets *pOk = 0 on
 * overflow; otherwise returns a * b and leaves *pOk unchanged (caller
 * seeds *pOk = 1).
 */
static uint64_t
b3311_mul_u64(uint64_t u64A, uint64_t u64B, int *pOk)
{
	if (u64A == 0ull || u64B == 0ull) {
		return 0ull;
	}
	if (u64A > (UINT64_MAX / u64B)) {
		*pOk = 0;
		return 0ull;
	}
	return u64A * u64B;
}

/* Binary exp; 0 on overflow; 0^0 → 1. */
static uint64_t
b3311_pow(uint64_t u64Base, uint64_t u64Exp)
{
	uint64_t u64Result;
	uint64_t u64B;
	uint64_t u64E;
	int iOk;

	if (u64Exp == 0ull) {
		return 1ull;
	}

	u64Result = 1ull;
	u64B = u64Base;
	u64E = u64Exp;
	iOk = 1;

	while (u64E > 0ull) {
		if ((u64E & 1ull) != 0ull) {
			u64Result = b3311_mul_u64(u64Result, u64B, &iOk);
			if (iOk == 0) {
				return 0ull;
			}
		}
		u64E >>= 1;
		if (u64E == 0ull) {
			break;
		}
		u64B = b3311_mul_u64(u64B, u64B, &iOk);
		if (iOk == 0) {
			return 0ull;
		}
	}

	return u64Result;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_pow_u — base^exp via binary exponentiation; 0 on overflow.
 *
 * base: power base
 * exp:  exponent (0 → 1 for any base, including 0^0)
 *
 * Returns base^exp when representable in uint64_t; 0 if any intermediate
 * product would exceed UINT64_MAX.
 */
uint64_t
gj_u64_pow_u(uint64_t u64Base, uint64_t u64Exp)
{
	(void)NULL;
	return b3311_pow(u64Base, u64Exp);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_pow_u(uint64_t u64Base, uint64_t u64Exp)
    __attribute__((alias("gj_u64_pow_u")));
