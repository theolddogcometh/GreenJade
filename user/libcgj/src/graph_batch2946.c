/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2946: next prime strictly greater than n (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_next_prime_u32_u(uint32_t n);
 *     — Smallest prime p with p > n. No such p in uint32 → 0.
 *       Uses local trial primality (does not call/redefine
 *       gj_is_prime_u32 / gj_is_prime_u32_u).
 *   uint32_t __gj_next_prime_u32_u  (alias)
 *   __libcgj_batch2946_marker = "libcgj-batch2946"
 *
 * CREATE-ONLY exclusive continuum wave (2941–2950). Distinct from
 * gj_next_prime_u32 (batch846) and gj_u64_next_prime_u (batch2508) —
 * unique gj_next_prime_u32_u name; no parent wires, no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2946_marker[] = "libcgj-batch2946";

/* ---- freestanding helpers ---------------------------------------------- */

/* Local trial primality; self-contained (no parent wire). */
static int
b2946_is_prime(uint32_t u32N)
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
 * gj_next_prime_u32_u — smallest prime strictly greater than n.
 *
 * If n is already at or beyond the last 32-bit prime (4294967291),
 * returns 0 (no next prime in the uint32 domain).
 */
uint32_t
gj_next_prime_u32_u(uint32_t u32N)
{
	uint32_t u32C;

	(void)NULL;
	if (u32N >= 4294967291u) {
		return 0u;
	}

	u32C = u32N + 1u;
	if (u32C <= 2u) {
		return 2u;
	}
	if ((u32C & 1u) == 0u) {
		u32C++; /* start at odd */
	}
	for (;;) {
		if (b2946_is_prime(u32C)) {
			return u32C;
		}
		if (u32C == 0xffffffffu) {
			return 0u;
		}
		/* next odd */
		if (u32C > 0xffffffffu - 2u) {
			return 0u;
		}
		u32C += 2u;
	}
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_next_prime_u32_u(uint32_t u32N)
    __attribute__((alias("gj_next_prime_u32_u")));
