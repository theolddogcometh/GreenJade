/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3516: Catalan number C_n as uint32_t.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_catalan_u(uint32_t n);
 *     - Return the n-th Catalan number C_n.
 *       C_0 = 1, C_{n} = C_{n-1} * (4n - 2) / (n + 1).
 *       Exact for n <= 19 (C_19 = 1767263190 fits uint32_t).
 *       Returns 0 for n >= 20 (overflow past UINT32_MAX).
 *   uint32_t __gj_u32_catalan_u  (alias)
 *   __libcgj_batch3516_marker = "libcgj-batch3516"
 *
 * Exclusive continuum CREATE-ONLY (3511-3520). Unique gj_u32_catalan_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3516_marker[] = "libcgj-batch3516";

/* C_n fits in uint32_t for n in 0..19 inclusive. */
#define B3516_CATALAN_U32_MAX_N  19u

/* ---- freestanding helpers ---------------------------------------------- */

/* Euclidean GCD for uint64 (fraction cancellation). */
static uint64_t
b3516_gcd(uint64_t u64A, uint64_t u64B)
{
	while (u64B != 0u) {
		uint64_t u64T = u64A % u64B;

		u64A = u64B;
		u64B = u64T;
	}
	return u64A;
}

/*
 * C_n via recurrence with exact cancellation so intermediates stay
 * integral and fit in uint64 for n <= 19:
 *   C_n = C_{n-1} * (4n - 2) / (n + 1)
 */
static uint32_t
b3516_catalan(uint32_t u32N)
{
	uint64_t u64C;
	uint32_t u32I;

	if (u32N > B3516_CATALAN_U32_MAX_N) {
		return 0u;
	}

	u64C = 1u;
	for (u32I = 1u; u32I <= u32N; u32I++) {
		uint64_t u64Num = (uint64_t)(4u * u32I - 2u);
		uint64_t u64Den = (uint64_t)u32I + 1u;
		uint64_t u64G;

		/* Cancel gcd(num, den) then gcd(c, den). */
		u64G = b3516_gcd(u64Num, u64Den);
		u64Num /= u64G;
		u64Den /= u64G;
		u64G = b3516_gcd(u64C, u64Den);
		u64C /= u64G;
		u64Den /= u64G;
		/* den must be 1 for Catalan integers. */
		u64C *= u64Num;
		if (u64Den != 1u) {
			u64C /= u64Den;
		}
	}
	return (uint32_t)u64C;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_catalan_u - n-th Catalan number as uint32_t.
 *
 * n: index (0-based)
 *
 * Returns C_n for n <= 19; 0 for n >= 20 (does not fit uint32_t).
 * No parent wires.
 */
uint32_t
gj_u32_catalan_u(uint32_t u32N)
{
	(void)NULL;
	return b3516_catalan(u32N);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_catalan_u(uint32_t u32N)
    __attribute__((alias("gj_u32_catalan_u")));
