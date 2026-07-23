/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch609: freestanding sum of a uint32_t array
 * into a uint64_t accumulator.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u32_sum(const uint32_t *a, size_t n);
 *     — Return a[0] + ... + a[n-1] as uint64_t. NULL a with n > 0
 *       yields 0; n == 0 yields 0. Widens each element before add so
 *       partial sums do not wrap at 32 bits.
 *   uint64_t __gj_u32_sum  (alias)
 *   __libcgj_batch609_marker = "libcgj-batch609"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch609_marker[] = "libcgj-batch609";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_sum — sum n uint32_t elements into a uint64_t.
 *
 * a: base (NULL with n > 0 → return 0)
 * n: element count
 *
 * Returns the sum of a[0..n) as uint64_t (0 if a is NULL or n is 0).
 */
uint64_t
gj_u32_sum(const uint32_t *pA, size_t cN)
{
	size_t i;
	uint64_t u64Sum;

	if (cN == 0u || pA == NULL) {
		return 0ull;
	}

	u64Sum = 0ull;
	for (i = 0u; i < cN; i++) {
		u64Sum += (uint64_t)pA[i];
	}
	return u64Sum;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u32_sum(const uint32_t *pA, size_t cN)
    __attribute__((alias("gj_u32_sum")));
