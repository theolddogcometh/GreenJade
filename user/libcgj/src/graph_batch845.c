/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch845: uint32 trial-division primality test.
 *
 * Surface (unique symbols):
 *   int gj_is_prime_u32(uint32_t n);
 *     — 1 if n is prime, 0 if composite or n < 2.
 *   int __gj_is_prime_u32  (alias)
 *   __libcgj_batch845_marker = "libcgj-batch845"
 *
 * Distinct from gj_is_prime_u64 (batch231) — no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch845_marker[] = "libcgj-batch845";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_is_prime_u32 — 1 if n is prime, else 0.
 *
 * Trial division up to floor(sqrt(n)) with a 2/3 wheel (skip multiples of
 * 2 and 3 after the small cases). Exact for the full uint32 range.
 */
int
gj_is_prime_u32(uint32_t u32N)
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
	 * Stop when i*i > n; i*i may overflow u32 near UINT32_MAX, so
	 * compare i > n/i when i != 0.
	 */
	for (u32I = 5u; u32I <= 0xffffffffu / u32I && u32I * u32I <= u32N;
	     u32I += 6u) {
		if (u32N % u32I == 0u || u32N % (u32I + 2u) == 0u) {
			return 0;
		}
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_is_prime_u32(uint32_t u32N)
    __attribute__((alias("gj_is_prime_u32")));
