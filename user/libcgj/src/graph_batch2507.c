/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2507: trial-division primality (uint64, soft).
 *
 * Surface (unique symbols):
 *   int gj_u64_is_prime_trial_u(uint64_t n);
 *     — 1 if n is prime, 0 if composite or n < 2.
 *       Trial divide up to floor(sqrt(n)) via soft integer isqrt.
 *       Exact for the full uint64 domain; intended primarily for small n
 *       (worst-case cost grows with sqrt(n)).
 *   int __gj_u64_is_prime_trial_u  (alias)
 *   __libcgj_batch2507_marker = "libcgj-batch2507"
 *
 * Post-2500 number-theory exclusive wave (2501–2510). Distinct from
 * gj_is_prime_u64 (batch231 MR) and gj_is_prime_u32 (batch845) —
 * unique gj_u64_is_prime_trial_u name; no multi-def. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2507_marker[] = "libcgj-batch2507";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Soft floor integer square root of n.
 * Binary search over r in [0, min(n, 2^32-1)]. floor(sqrt(UINT64_MAX))
 * is 4294967295, so mid*mid never overflows uint64 under that cap.
 */
static uint64_t
b2507_isqrt_soft(uint64_t u64N)
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

/*
 * Trial primality: check divisors up to soft isqrt(n).
 * 2/3 wheel after small cases.
 */
static int
b2507_is_prime_trial(uint64_t u64N)
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

	u64Lim = b2507_isqrt_soft(u64N);

	/*
	 * Check i = 5,7,11,13,... via (i, i+2) steps of 6.
	 * Stop when i > lim (i+2 may exceed lim; still safe % checks).
	 */
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
 * gj_u64_is_prime_trial_u — 1 if n is prime, else 0.
 *
 * n: candidate (trial division to soft floor(sqrt(n)))
 *
 * Best used for small n; still correct for large n at higher cost.
 */
int
gj_u64_is_prime_trial_u(uint64_t u64N)
{
	(void)NULL;
	return b2507_is_prime_trial(u64N);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u64_is_prime_trial_u(uint64_t u64N)
    __attribute__((alias("gj_u64_is_prime_trial_u")));
