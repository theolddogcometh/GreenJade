/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1404: O(n^2) inversion count for small n.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u32_count_inversions_n(const uint32_t *a, size_t n);
 *     — Count pairs (i, j) with 0 <= i < j < n and a[i] > a[j].
 *       Soft n cap B1404_N_CAP (4096): larger n is truncated to the
 *       first B1404_N_CAP elements. a == NULL or n == 0 → 0.
 *   uint64_t __gj_u32_count_inversions_n  (alias)
 *   __libcgj_batch1404_marker = "libcgj-batch1404"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1404_marker[] = "libcgj-batch1404";

/* Soft cap for O(n^2) scan — small-n helper only. */
#define B1404_N_CAP 4096u

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b1404_count_inv — count inversions over a[0..n) with n already capped.
 * Caller guarantees a != NULL when n > 0.
 */
static uint64_t
b1404_count_inv(const uint32_t *a, size_t n)
{
	uint64_t u64Cnt;
	size_t i;
	size_t j;

	u64Cnt = 0ull;
	for (i = 0u; i < n; i++) {
		for (j = i + 1u; j < n; j++) {
			if (a[i] > a[j]) {
				u64Cnt++;
			}
		}
	}
	return u64Cnt;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_count_inversions_n — O(n^2) inversion count, n soft-capped.
 *
 * Edge cases:
 *   a == NULL || n == 0 → 0
 *   n > B1404_N_CAP → scan only first B1404_N_CAP elements
 */
uint64_t
gj_u32_count_inversions_n(const uint32_t *a, size_t n)
{
	size_t cUse;

	if (a == NULL || n == 0u) {
		return 0ull;
	}

	cUse = (n > (size_t)B1404_N_CAP) ? (size_t)B1404_N_CAP : n;
	return b1404_count_inv(a, cUse);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u32_count_inversions_n(const uint32_t *a, size_t n)
    __attribute__((alias("gj_u32_count_inversions_n")));
