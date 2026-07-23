/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch385: unsigned 64-bit integer power (binary exp).
 *
 * Surface (unique symbols):
 *   uint64_t gj_pow_u64(uint64_t base, unsigned exp);
 *     — Compute base^exp by binary (exponentiation-by-squaring).
 *       gj_pow_u64(0, 0) == 1 (0^0 defined as 1).
 *       On any intermediate multiply overflow past UINT64_MAX, return 0.
 *   uint64_t __gj_pow_u64  (alias)
 *   __libcgj_batch385_marker = "libcgj-batch385"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch385_marker[] = "libcgj-batch385";

/* ---- helpers ----------------------------------------------------------- */

/*
 * Multiply with overflow detection. Returns 0 and sets *pOk = 0 on
 * overflow; otherwise returns a * b and leaves *pOk unchanged (caller
 * seeds *pOk = 1).
 */
static uint64_t
b385_mul_u64(uint64_t u64A, uint64_t u64B, int *pOk)
{
	if (u64A == 0u || u64B == 0u) {
		return 0u;
	}
	if (u64A > (UINT64_MAX / u64B)) {
		*pOk = 0;
		return 0u;
	}
	return u64A * u64B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pow_u64 — base^exp via binary exponentiation; 0 on overflow; 0^0=1.
 *
 * Classic square-and-multiply over uint64_t with checked multiplies.
 */
uint64_t
gj_pow_u64(uint64_t u64Base, unsigned uExp)
{
	uint64_t u64Result;
	uint64_t u64B;
	unsigned uE;
	int iOk;

	/* Any base^0 == 1, including 0^0. */
	if (uExp == 0u) {
		return 1u;
	}

	u64Result = 1u;
	u64B = u64Base;
	uE = uExp;
	iOk = 1;

	while (uE > 0u) {
		if ((uE & 1u) != 0u) {
			u64Result = b385_mul_u64(u64Result, u64B, &iOk);
			if (iOk == 0) {
				return 0u;
			}
		}
		uE >>= 1;
		if (uE == 0u) {
			break;
		}
		u64B = b385_mul_u64(u64B, u64B, &iOk);
		if (iOk == 0) {
			return 0u;
		}
	}

	return u64Result;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_pow_u64(uint64_t u64Base, unsigned uExp)
    __attribute__((alias("gj_pow_u64")));
