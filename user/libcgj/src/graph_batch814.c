/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch814: freestanding minimum of a uint64_t array.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_min_arr(const uint64_t *a, size_t n);
 *     — Return the minimum of a[0..n). a == NULL or n == 0 → 0.
 *   uint64_t __gj_u64_min_arr  (alias)
 *   __libcgj_batch814_marker = "libcgj-batch814"
 *
 * Distinct from gj_i64_min_arr (batch811) / gj_min_arr_i64 (batch409) —
 * unique gj_u64_min_arr surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch814_marker[] = "libcgj-batch814";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_min_arr — least value in a[0..n).
 *
 * a: base (NULL → 0)
 * n: element count (0 → 0)
 */
uint64_t
gj_u64_min_arr(const uint64_t *pA, size_t n)
{
	size_t i;
	uint64_t u64Min;

	if (pA == NULL || n == 0u) {
		return 0ull;
	}

	u64Min = pA[0];
	for (i = 1u; i < n; i++) {
		if (pA[i] < u64Min) {
			u64Min = pA[i];
		}
	}
	return u64Min;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_min_arr(const uint64_t *pA, size_t n)
    __attribute__((alias("gj_u64_min_arr")));
