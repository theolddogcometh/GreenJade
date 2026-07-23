/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch571 (renamed surface to avoid multi-def with earlier batch): Fibonacci F(n) in uint64_t with overflow guard.
 *
 * Surface (unique symbols):
 *   uint64_t gj_fibonacci_u64(unsigned n);
 *     — F(0)=0, F(1)=1, F(n)=F(n-1)+F(n-2). Returns exact F(n) for
 *       n <= 93 (F(93) is the largest Fibonacci that fits in uint64_t).
 *       For n > 93 returns 0 (overflow sentinel).
 *   __gj_fibonacci_u64  (alias)
 *   __libcgj_batch571_marker = "libcgj-batch571"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch571_marker[] = "libcgj-batch571";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fibonacci_u64 — nth Fibonacci number in uint64_t.
 *
 * F(0) = 0, F(1) = 1.
 * For n > 93, F(n) does not fit in 64 bits; return 0.
 * Iterative O(n) accumulation; n is bounded by 93 in the success path.
 */
uint64_t
gj_fibonacci_u64(unsigned n)
{
	uint64_t a;
	uint64_t b;
	unsigned i;

	if (n > 93u) {
		return 0ull;
	}
	if (n == 0u) {
		return 0ull;
	}
	if (n == 1u) {
		return 1ull;
	}

	a = 0ull;
	b = 1ull;
	for (i = 2u; i <= n; i++) {
		uint64_t c;

		c = a + b;
		a = b;
		b = c;
	}
	return b;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_fibonacci_u64(unsigned n)
    __attribute__((alias("gj_fibonacci_u64")));
