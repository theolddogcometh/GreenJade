/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch816: freestanding sum of a uint64_t array.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_sum_arr(const uint64_t *a, size_t n);
 *     — Return a[0] + ... + a[n-1] as uint64_t (wraps mod 2^64).
 *       a == NULL or n == 0 → 0.
 *   uint64_t __gj_u64_sum_arr  (alias)
 *   __libcgj_batch816_marker = "libcgj-batch816"
 *
 * Distinct from gj_u32_sum (batch609) / gj_u16_sum (batch638) /
 * gj_i64_sum_arr (batch813) — unique gj_u64_sum_arr surface only; no
 * multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch816_marker[] = "libcgj-batch816";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_sum_arr — sum n uint64_t elements (wrapping add).
 *
 * a: base (NULL with n > 0 → return 0)
 * n: element count
 *
 * Returns the sum of a[0..n) as uint64_t (0 if a is NULL or n is 0).
 */
uint64_t
gj_u64_sum_arr(const uint64_t *pA, size_t n)
{
	size_t i;
	uint64_t u64Sum;

	if (n == 0u || pA == NULL) {
		return 0ull;
	}

	u64Sum = 0ull;
	for (i = 0u; i < n; i++) {
		u64Sum += pA[i];
	}
	return u64Sum;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_sum_arr(const uint64_t *pA, size_t n)
    __attribute__((alias("gj_u64_sum_arr")));
