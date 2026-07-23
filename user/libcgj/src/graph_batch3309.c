/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3309: unsigned 32-bit integer power (binary exp).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_pow_u(uint32_t base, uint32_t exp);
 *     - Compute base^exp by binary (exponentiation-by-squaring).
 *       gj_u32_pow_u(0, 0) == 1 (0^0 defined as 1).
 *       On any intermediate multiply overflow past UINT32_MAX, return 0.
 *   uint32_t __gj_u32_pow_u  (alias)
 *   __libcgj_batch3309_marker = "libcgj-batch3309"
 *
 * Exclusive continuum CREATE-ONLY (3301-3310). Distinct from
 * gj_pow_u64 (batch385) — unique u32 power surface; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3309_marker[] = "libcgj-batch3309";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Multiply with overflow detection. Returns 0 and sets *pOk = 0 on
 * overflow; otherwise returns a * b and leaves *pOk unchanged (caller
 * seeds *pOk = 1).
 */
static uint32_t
b3309_mul_u32(uint32_t uA, uint32_t uB, int *pOk)
{
	if (uA == 0u || uB == 0u) {
		return 0u;
	}
	if (uA > (UINT32_MAX / uB)) {
		*pOk = 0;
		return 0u;
	}
	return uA * uB;
}

/* base^exp via binary exponentiation; 0 on overflow; 0^0 = 1. */
static uint32_t
b3309_pow(uint32_t uBase, uint32_t uExp)
{
	uint32_t uResult;
	uint32_t uB;
	uint32_t uE;
	int iOk;

	/* Any base^0 == 1, including 0^0. */
	if (uExp == 0u) {
		return 1u;
	}

	uResult = 1u;
	uB = uBase;
	uE = uExp;
	iOk = 1;

	while (uE > 0u) {
		if ((uE & 1u) != 0u) {
			uResult = b3309_mul_u32(uResult, uB, &iOk);
			if (iOk == 0) {
				return 0u;
			}
		}
		uE >>= 1;
		if (uE == 0u) {
			break;
		}
		uB = b3309_mul_u32(uB, uB, &iOk);
		if (iOk == 0) {
			return 0u;
		}
	}

	return uResult;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_pow_u - base^exp via binary exponentiation.
 *
 * base: unsigned 32-bit base
 * exp:  unsigned 32-bit exponent
 *
 * Returns base^exp; 0 on overflow past UINT32_MAX; 0^0 defined as 1.
 * No parent wires.
 */
uint32_t
gj_u32_pow_u(uint32_t base, uint32_t exp)
{
	(void)NULL;
	return b3309_pow(base, exp);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_pow_u(uint32_t base, uint32_t exp)
    __attribute__((alias("gj_u32_pow_u")));
