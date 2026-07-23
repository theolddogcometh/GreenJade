/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2064: freestanding bitwise AND-reduce of a
 * uint32_t array (n-bound exclusive surface).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_and_n(const uint32_t *a, size_t n);
 *     - a[0] & a[1] & ... & a[n-1]. n==0 -> all-ones (AND identity).
 *       a==NULL with n>0 -> 0.
 *   uint32_t __gj_u32_and_n  (alias)
 *   __libcgj_batch2064_marker = "libcgj-batch2064"
 *
 * Distinct from gj_u32_and_reduce (batch1246) - unique gj_u32_and_n
 * name only; no multi-def. Array u32 exclusive wave (2061-2070).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2064_marker[] = "libcgj-batch2064";

/* ---- freestanding helpers ---------------------------------------------- */

/* AND-fold a[0..cN) starting from all-ones. Caller: pA non-NULL, cN>0. */
static uint32_t
b2064_and(const uint32_t *pA, size_t cN)
{
	size_t i;
	uint32_t u32Acc;

	u32Acc = ~0u;
	for (i = 0u; i < cN; i++) {
		u32Acc &= pA[i];
	}
	return u32Acc;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_and_n - fold array with bitwise AND (identity ~0).
 *
 * a: base (NULL with n > 0 -> return 0)
 * n: element count (0 -> ~0u, AND identity)
 *
 * Returns a[0] & ... & a[n-1].
 */
uint32_t
gj_u32_and_n(const uint32_t *a, size_t n)
{
	(void)NULL;
	if (n == 0u) {
		return ~0u;
	}
	if (a == NULL) {
		return 0u;
	}
	return b2064_and(a, n);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_and_n(const uint32_t *a, size_t n)
    __attribute__((alias("gj_u32_and_n")));
