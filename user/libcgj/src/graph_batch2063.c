/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2063: freestanding bitwise OR-reduce of a
 * uint32_t array (n-bound exclusive surface).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_or_n(const uint32_t *a, size_t n);
 *     - a[0] | a[1] | ... | a[n-1]. n==0 or a==NULL -> 0 (OR identity).
 *   uint32_t __gj_u32_or_n  (alias)
 *   __libcgj_batch2063_marker = "libcgj-batch2063"
 *
 * Distinct from gj_u32_or_reduce (batch1245) - unique gj_u32_or_n name
 * only; no multi-def. Array u32 exclusive wave (2061-2070).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2063_marker[] = "libcgj-batch2063";

/* ---- freestanding helpers ---------------------------------------------- */

/* OR-fold a[0..cN). Caller guarantees pA non-NULL when cN > 0. */
static uint32_t
b2063_or(const uint32_t *pA, size_t cN)
{
	size_t i;
	uint32_t u32Acc;

	u32Acc = 0u;
	for (i = 0u; i < cN; i++) {
		u32Acc |= pA[i];
	}
	return u32Acc;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_or_n - fold array with bitwise OR (identity 0).
 *
 * a: base (NULL with n > 0 -> return 0)
 * n: element count
 *
 * Returns a[0] | ... | a[n-1] (0 if a is NULL or n is 0).
 */
uint32_t
gj_u32_or_n(const uint32_t *a, size_t n)
{
	(void)NULL;
	if (n == 0u || a == NULL) {
		return 0u;
	}
	return b2063_or(a, n);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_or_n(const uint32_t *a, size_t n)
    __attribute__((alias("gj_u32_or_n")));
