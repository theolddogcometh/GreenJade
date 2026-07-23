/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2509: previous prime strictly less than n (u64).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_prev_prime_u(uint64_t n);
 *     — Largest prime p with p < n. No such p (n <= 2) → 0.
 *       Uses local soft trial primality (does not call/redefine
 *       gj_u64_is_prime_trial_u / gj_u64_next_prime_u).
 *   uint64_t __gj_u64_prev_prime_u  (alias)
 *   __libcgj_batch2509_marker = "libcgj-batch2509"
 *
 * Post-2500 number-theory exclusive wave (2501–2510). Unique
 * gj_u64_prev_prime_u surface; no multi-def. Intended primarily for
 * small n (trial cost).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2509_marker[] = "libcgj-batch2509";

/* ---- freestanding helpers ---------------------------------------------- */

/* Soft floor integer square root. */
static uint64_t
b2509_isqrt_soft(uint64_t u64N)
{
	uint64_t u64Lo;
	uint64_t u64Hi;
	uint64_t u64Mid;
	uint64_t u64Sq;

	if (u64N <= 1ull) {
		return u64N;
	}

	u64Lo = 1ull;
	u64Hi = (u64N < 0xffffffffull) ? u64N : 0xffffffffull;

	while (u64Lo < u64Hi) {
		u64Mid = u64Lo + ((u64Hi - u64Lo + 1ull) >> 1);
		u64Sq = u64Mid * u64Mid;
		if (u64Sq <= u64N) {
			u64Lo = u64Mid;
		} else {
			u64Hi = u64Mid - 1ull;
		}
	}
	return u64Lo;
}

static int
b2509_is_prime(uint64_t u64N)
{
	uint64_t u64Lim;
	uint64_t u64I;

	if (u64N < 2ull) {
		return 0;
	}
	if (u64N == 2ull || u64N == 3ull) {
		return 1;
	}
	if ((u64N & 1ull) == 0ull || u64N % 3ull == 0ull) {
		return 0;
	}

	u64Lim = b2509_isqrt_soft(u64N);
	for (u64I = 5ull; u64I <= u64Lim; u64I += 6ull) {
		if (u64N % u64I == 0ull) {
			return 0;
		}
		if ((u64I + 2ull) <= u64Lim && u64N % (u64I + 2ull) == 0ull) {
			return 0;
		}
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_prev_prime_u — largest prime strictly less than n.
 *
 * n <= 2 → 0 (no previous prime in the non-negative domain).
 * n == 3 → 2. Otherwise scan downward through odds.
 */
uint64_t
gj_u64_prev_prime_u(uint64_t u64N)
{
	uint64_t u64C;

	(void)NULL;

	if (u64N <= 2ull) {
		return 0ull;
	}
	if (u64N == 3ull) {
		return 2ull;
	}

	/* Start at n-1, force odd. */
	u64C = u64N - 1ull;
	if ((u64C & 1ull) == 0ull) {
		u64C--;
	}

	for (;;) {
		if (b2509_is_prime(u64C)) {
			return u64C;
		}
		if (u64C <= 3ull) {
			/* 3 was not hit as start only when n>3; fall to 2. */
			return 2ull;
		}
		u64C -= 2ull;
	}
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_prev_prime_u(uint64_t u64N)
    __attribute__((alias("gj_u64_prev_prime_u")));
