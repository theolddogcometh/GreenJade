/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2061: freestanding sum of a uint32_t array
 * into a uint64_t accumulator (n-bound exclusive surface).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u32_sum_n(const uint32_t *a, size_t n);
 *     - Return a[0] + ... + a[n-1] as uint64_t. NULL a with n > 0
 *       yields 0; n == 0 yields 0. Widens each element before add so
 *       partial sums do not wrap at 32 bits.
 *   uint64_t __gj_u32_sum_n  (alias)
 *   __libcgj_batch2061_marker = "libcgj-batch2061"
 *
 * Distinct from gj_u32_sum (batch609) - unique gj_u32_sum_n name only;
 * no multi-def. Array u32 exclusive wave (2061-2070).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2061_marker[] = "libcgj-batch2061";

/* ---- freestanding helpers ---------------------------------------------- */

/* Sum a[0..cN) into uint64_t. Caller guarantees pA non-NULL when cN > 0. */
static uint64_t
b2061_sum(const uint32_t *pA, size_t cN)
{
	size_t i;
	uint64_t u64Sum;

	u64Sum = 0ull;
	for (i = 0u; i < cN; i++) {
		u64Sum += (uint64_t)pA[i];
	}
	return u64Sum;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_sum_n - sum n uint32_t elements into a uint64_t.
 *
 * a: base (NULL with n > 0 -> return 0)
 * n: element count
 *
 * Returns the sum of a[0..n) as uint64_t (0 if a is NULL or n is 0).
 */
uint64_t
gj_u32_sum_n(const uint32_t *a, size_t n)
{
	(void)NULL;
	if (n == 0u || a == NULL) {
		return 0ull;
	}
	return b2061_sum(a, n);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u32_sum_n(const uint32_t *a, size_t n)
    __attribute__((alias("gj_u32_sum_n")));
