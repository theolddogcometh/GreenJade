/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4177: freestanding nondecreasing uint32_t
 * order test.
 *
 * Surface (unique symbols):
 *   int gj_u32_arr_is_sorted_u(const uint32_t *a, size_t n);
 *     - Return 1 if a[0..n) is sorted nondecreasing (a[i] <= a[i+1]),
 *       else 0. a == NULL with n >= 2 → 0; n < 2 → 1 (vacuously sorted).
 *   int __gj_u32_arr_is_sorted_u  (alias)
 *   __libcgj_batch4177_marker = "libcgj-batch4177"
 *
 * Exclusive continuum CREATE-ONLY (4171-4180). Distinct from
 * gj_is_sorted_u32 (batch405) and gj_u32_is_sorted_asc (batch665) —
 * unique gj_u32_arr_is_sorted_u surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4177_marker[] = "libcgj-batch4177";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b4177_is_sorted(const uint32_t *pA, size_t cN)
{
	size_t i;

	if (cN < 2u) {
		return 1;
	}
	if (pA == NULL) {
		return 0;
	}

	for (i = 1u; i < cN; i++) {
		if (pA[i] < pA[i - 1u]) {
			return 0;
		}
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_arr_is_sorted_u - 1 if nondecreasing, else 0.
 *
 * a: base (NULL with n >= 2 → 0)
 * n: element count (n < 2 → 1, vacuously sorted)
 *
 * Equal adjacent elements allowed (nondecreasing, not strict).
 * Self-contained; no parent wires.
 */
int
gj_u32_arr_is_sorted_u(const uint32_t *pA, size_t cN)
{
	(void)NULL;
	return b4177_is_sorted(pA, cN);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u32_arr_is_sorted_u(const uint32_t *pA, size_t cN)
    __attribute__((alias("gj_u32_arr_is_sorted_u")));
