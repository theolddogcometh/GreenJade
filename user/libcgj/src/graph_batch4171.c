/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4171: freestanding sum of a uint32_t array.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_arr_sum_u(const uint32_t *a, size_t n);
 *     - Return a[0] + ... + a[n-1] as uint32_t (wraps mod 2^32).
 *       a == NULL or n == 0 → 0.
 *   uint32_t __gj_u32_arr_sum_u  (alias)
 *   __libcgj_batch4171_marker = "libcgj-batch4171"
 *
 * Exclusive continuum CREATE-ONLY (4171-4180: u32_arr_sum_u,
 * u32_arr_xor_u, u32_arr_or_u, u32_arr_and_u, u32_arr_min_u,
 * u32_arr_max_u, u32_arr_is_sorted_u, u32_arr_count_eq_u,
 * u32_arr_find_u, batch_id_4180). Distinct from gj_u64_sum_arr
 * (batch816) and gj_u32_sum (batch609) — unique gj_u32_arr_sum_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4171_marker[] = "libcgj-batch4171";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4171_sum(const uint32_t *pA, size_t cN)
{
	size_t i;
	uint32_t u32Sum;

	if (pA == NULL || cN == 0u) {
		return 0u;
	}

	u32Sum = 0u;
	for (i = 0u; i < cN; i++) {
		u32Sum += pA[i];
	}
	return u32Sum;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_arr_sum_u - sum n uint32_t elements (wrapping add).
 *
 * a: base (NULL → return 0)
 * n: element count (0 → return 0)
 *
 * Returns the sum of a[0..n) as uint32_t (wraps mod 2^32).
 * Self-contained; no parent wires.
 */
uint32_t
gj_u32_arr_sum_u(const uint32_t *pA, size_t cN)
{
	(void)NULL;
	return b4171_sum(pA, cN);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_arr_sum_u(const uint32_t *pA, size_t cN)
    __attribute__((alias("gj_u32_arr_sum_u")));
