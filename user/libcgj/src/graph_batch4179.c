/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4179: freestanding linear search for a
 * uint32_t value.
 *
 * Surface (unique symbols):
 *   size_t gj_u32_arr_find_u(const uint32_t *a, size_t n, uint32_t v);
 *     - Return the least index i with a[i] == v, or n if not found
 *       (also when a == NULL or n == 0).
 *   size_t __gj_u32_arr_find_u  (alias)
 *   __libcgj_batch4179_marker = "libcgj-batch4179"
 *
 * Exclusive continuum CREATE-ONLY (4171-4180). Distinct from
 * gj_find_u32 (batch403) — unique gj_u32_arr_find_u surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4179_marker[] = "libcgj-batch4179";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b4179_find(const uint32_t *pA, size_t cN, uint32_t u32V)
{
	size_t i;

	if (pA == NULL || cN == 0u) {
		return cN;
	}

	for (i = 0u; i < cN; i++) {
		if (pA[i] == u32V) {
			return i;
		}
	}
	return cN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_arr_find_u - first index of v in a[0..n), or n if absent.
 *
 * a: base (NULL → return n)
 * n: element count (0 → return 0; not-found sentinel is n)
 * v: value to match
 *
 * Multiple matches → lowest index. Self-contained; no parent wires.
 */
size_t
gj_u32_arr_find_u(const uint32_t *pA, size_t cN, uint32_t u32V)
{
	(void)NULL;
	return b4179_find(pA, cN, u32V);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_u32_arr_find_u(const uint32_t *pA, size_t cN, uint32_t u32V)
    __attribute__((alias("gj_u32_arr_find_u")));
