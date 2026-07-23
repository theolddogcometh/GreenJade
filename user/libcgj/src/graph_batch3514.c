/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3514: factorial modulo m for uint32_t.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_fact_mod_u(uint32_t n, uint32_t m);
 *     - Return n! mod m. m == 0 → 0; m == 1 → 0; 0! == 1.
 *       Early-exit 0 when a factor of m appears in 1..n (n >= m > 1
 *       implies n! ≡ 0, but also for composite m when a full set of
 *       prime factors of m is covered — implemented via direct product).
 *   uint32_t __gj_u32_fact_mod_u  (alias)
 *   __libcgj_batch3514_marker = "libcgj-batch3514"
 *
 * Exclusive continuum CREATE-ONLY (3511-3520). Unique gj_u32_fact_mod_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3514_marker[] = "libcgj-batch3514";

/* ---- freestanding helpers ---------------------------------------------- */

/* (a * b) mod m for m > 1; product fits in uint64. */
static uint32_t
b3514_mulmod(uint32_t u32A, uint32_t u32B, uint32_t u32M)
{
	return (uint32_t)(((uint64_t)u32A * (uint64_t)u32B) % (uint64_t)u32M);
}

/*
 * n! mod m, m > 1. Once the running product hits 0 mod m, stay 0.
 * For n >= m the product includes m itself when m is prime, but for
 * general m we still multiply through — if n is huge this is O(n);
 * early-out when r == 0 is enough in practice for graph callers.
 */
static uint32_t
b3514_fact_mod(uint32_t u32N, uint32_t u32M)
{
	uint32_t u32R;
	uint32_t u32I;

	u32R = 1u;
	for (u32I = 2u; u32I <= u32N; u32I++) {
		u32R = b3514_mulmod(u32R, u32I % u32M, u32M);
		if (u32R == 0u) {
			return 0u;
		}
		/* When i has grown past m for prime m, done; also bound. */
		if (u32I >= u32M && (u32M > 1u)) {
			/*
			 * Among 1..m is a multiple of every prime power in
			 * m only for some m; safe early-out: if n >= m the
			 * product 1..m is 0 mod m always (m itself appears).
			 */
			return 0u;
		}
	}
	return u32R;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_fact_mod_u - n! modulo m.
 *
 * n: non-negative integer (0! = 1)
 * m: modulus
 *
 * Returns n! mod m in [0, m) when m > 1; 0 when m <= 1.
 * No parent wires.
 */
uint32_t
gj_u32_fact_mod_u(uint32_t u32N, uint32_t u32M)
{
	(void)NULL;

	if (u32M <= 1u) {
		return 0u;
	}
	if (u32N <= 1u) {
		return 1u % u32M;
	}
	return b3514_fact_mod(u32N, u32M);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_fact_mod_u(uint32_t u32N, uint32_t u32M)
    __attribute__((alias("gj_u32_fact_mod_u")));
