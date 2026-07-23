/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1405: verify ascending argsort via index array.
 *
 * Surface (unique symbols):
 *   int gj_u32_args_sorted_asc(const uint32_t *a, const uint32_t *idx,
 *                              size_t n);
 *     — Return 1 (OK) if idx is a valid ascending argsort of a[0..n):
 *       for all i in [0, n-1), a[idx[i]] <= a[idx[i+1]], and every
 *       idx[k] < n. Else 0 (FAIL). a/idx NULL with n > 1 → 0;
 *       n <= 1 → 1 (vacuous). Does not require idx to be a permutation
 *       of 0..n-1 (bounds only).
 *   int __gj_u32_args_sorted_asc  (alias)
 *   __libcgj_batch1405_marker = "libcgj-batch1405"
 *
 * Distinct from gj_u32_is_sorted_asc (batch665). Unique gj_ names only;
 * do not multi-def prior batch symbols.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1405_marker[] = "libcgj-batch1405";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b1405_idx_ok — 1 if every idx[k] is in [0, n).
 */
static int
b1405_idx_ok(const uint32_t *idx, size_t n)
{
	size_t k;

	for (k = 0u; k < n; k++) {
		if ((size_t)idx[k] >= n) {
			return 0;
		}
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_args_sorted_asc — 1 if a[idx[*]] is nondecreasing, else 0.
 *
 * Edge cases:
 *   n <= 1 → 1
 *   a == NULL || idx == NULL with n > 1 → 0
 *   any idx[k] >= n → 0
 */
int
gj_u32_args_sorted_asc(const uint32_t *a, const uint32_t *idx, size_t n)
{
	size_t i;

	if (n <= 1u) {
		return 1;
	}
	if (a == NULL || idx == NULL) {
		return 0;
	}
	if (!b1405_idx_ok(idx, n)) {
		return 0;
	}

	for (i = 1u; i < n; i++) {
		if (a[idx[i]] < a[idx[i - 1u]]) {
			return 0;
		}
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u32_args_sorted_asc(const uint32_t *a, const uint32_t *idx,
    size_t n)
    __attribute__((alias("gj_u32_args_sorted_asc")));
