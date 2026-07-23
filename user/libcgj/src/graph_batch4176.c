/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4176: freestanding maximum of a uint32_t
 * array.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_arr_max_u(const uint32_t *a, size_t n);
 *     - Return the greatest element of a[0..n). a == NULL or n == 0 → 0.
 *   uint32_t __gj_u32_arr_max_u  (alias)
 *   __libcgj_batch4176_marker = "libcgj-batch4176"
 *
 * Exclusive continuum CREATE-ONLY (4171-4180). Distinct from
 * gj_max_arr_i64 (batch410) — unique gj_u32_arr_max_u surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4176_marker[] = "libcgj-batch4176";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4176_max(const uint32_t *pA, size_t cN)
{
	size_t i;
	uint32_t u32Max;

	if (pA == NULL || cN == 0u) {
		return 0u;
	}

	u32Max = pA[0];
	for (i = 1u; i < cN; i++) {
		if (pA[i] > u32Max) {
			u32Max = pA[i];
		}
	}
	return u32Max;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_arr_max_u - greatest value in a[0..n).
 *
 * a: base (NULL → return 0)
 * n: element count (0 → return 0)
 *
 * Returns the maximum of a[0..n). Self-contained; no parent wires.
 */
uint32_t
gj_u32_arr_max_u(const uint32_t *pA, size_t cN)
{
	(void)NULL;
	return b4176_max(pA, cN);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_arr_max_u(const uint32_t *pA, size_t cN)
    __attribute__((alias("gj_u32_arr_max_u")));
