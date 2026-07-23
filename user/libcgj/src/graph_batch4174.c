/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4174: freestanding AND-reduce of a uint32_t
 * array.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_arr_and_u(const uint32_t *a, size_t n);
 *     - Return a[0] & ... & a[n-1]. n == 0 → 0xffffffff (AND identity).
 *       a == NULL with n > 0 → 0xffffffff (no deref).
 *   uint32_t __gj_u32_arr_and_u  (alias)
 *   __libcgj_batch4174_marker = "libcgj-batch4174"
 *
 * Exclusive continuum CREATE-ONLY (4171-4180). Unique gj_u32_arr_and_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4174_marker[] = "libcgj-batch4174";

/* AND-reduce identity (all bits set). */
#define B4174_AND_ID  ((uint32_t)0xffffffffu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4174_and(const uint32_t *pA, size_t cN)
{
	size_t i;
	uint32_t u32Acc;

	if (pA == NULL || cN == 0u) {
		return B4174_AND_ID;
	}

	u32Acc = B4174_AND_ID;
	for (i = 0u; i < cN; i++) {
		u32Acc &= pA[i];
	}
	return u32Acc;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_arr_and_u - AND-reduce n uint32_t elements.
 *
 * a: base (NULL → return 0xffffffff)
 * n: element count (0 → return 0xffffffff, AND identity)
 *
 * Returns a[0] & ... & a[n-1]. Self-contained; no parent wires.
 */
uint32_t
gj_u32_arr_and_u(const uint32_t *pA, size_t cN)
{
	(void)NULL;
	return b4174_and(pA, cN);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_arr_and_u(const uint32_t *pA, size_t cN)
    __attribute__((alias("gj_u32_arr_and_u")));
