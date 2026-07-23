/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2866: modular sum of a uint64_t array (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_arr_u64_sum_u(const uint64_t *a, size_t n);
 *     — Return the sum of a[0..n) as uint64_t (modular wrap on overflow).
 *       a == NULL or n == 0 → 0. No __int128; pure uint64 modular add.
 *   uint64_t __gj_arr_u64_sum_u  (alias)
 *   __libcgj_batch2866_marker = "libcgj-batch2866"
 *
 * Bitmap/array exclusive wave (2861-2870). Distinct from gj_arr_u32_sum_u
 * (batch2863) and gj_sum_u32 (batch404) — unique gj_arr_u64_sum_u only.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2866_marker[] = "libcgj-batch2866";

/* ---- freestanding helpers ---------------------------------------------- */

/* Modular sum of pA[0..cN). Caller guarantees pA non-NULL and cN > 0. */
static uint64_t
b2866_sum(const uint64_t *pA, size_t cN)
{
	size_t i;
	uint64_t u64Sum;

	u64Sum = 0u;
	for (i = 0u; i < cN; i++) {
		u64Sum += pA[i];
	}
	return u64Sum;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_arr_u64_sum_u — modular sum of a[0..n).
 *
 * a: base (NULL → 0)
 * n: element count (0 → 0)
 *
 * Overflow past UINT64_MAX wraps (unsigned arithmetic). Does not call libc.
 */
uint64_t
gj_arr_u64_sum_u(const uint64_t *pA, size_t cN)
{
	(void)NULL;
	if (pA == NULL || cN == 0u) {
		return 0u;
	}
	return b2866_sum(pA, cN);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_arr_u64_sum_u(const uint64_t *pA, size_t cN)
    __attribute__((alias("gj_arr_u64_sum_u")));
