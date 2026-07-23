/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2945: uint32 trial-division primality (_u).
 *
 * Surface (unique symbols):
 *   int gj_is_prime_u32_u(uint32_t n);
 *     — 1 if n is prime, 0 if composite or n < 2.
 *   int __gj_is_prime_u32_u  (alias)
 *   __libcgj_batch2945_marker = "libcgj-batch2945"
 *
 * CREATE-ONLY exclusive continuum wave (2941–2950). Distinct from
 * gj_is_prime_u32 (batch845), gj_is_prime_u64 (batch231), and
 * gj_u64_is_prime_trial_u (batch2507) — unique gj_is_prime_u32_u name;
 * no parent wires, no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2945_marker[] = "libcgj-batch2945";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff u32N is prime (trial division with 2/3 wheel). */
static int
b2945_is_prime(uint32_t u32N)
{
	uint32_t u32I;

	if (u32N < 2u) {
		return 0;
	}
	if (u32N == 2u || u32N == 3u) {
		return 1;
	}
	if ((u32N & 1u) == 0u || u32N % 3u == 0u) {
		return 0;
	}

	/*
	 * Check i = 5,7,11,13,... via (i, i+2) steps of 6.
	 * Stop when i*i > n; guard i*i overflow near UINT32_MAX via
	 * i <= UINT32_MAX / i.
	 */
	for (u32I = 5u; u32I <= 0xffffffffu / u32I && u32I * u32I <= u32N;
	     u32I += 6u) {
		if (u32N % u32I == 0u || u32N % (u32I + 2u) == 0u) {
			return 0;
		}
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_is_prime_u32_u — 1 if n is prime, else 0.
 *
 * Trial division up to floor(sqrt(n)) with a 2/3 wheel. Exact for the
 * full uint32 range.
 */
int
gj_is_prime_u32_u(uint32_t u32N)
{
	(void)NULL;
	return b2945_is_prime(u32N);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_is_prime_u32_u(uint32_t u32N)
    __attribute__((alias("gj_is_prime_u32_u")));
