/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3517: Fibonacci F(n) mod m for uint32_t index.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_fib_mod_u(uint32_t n, uint32_t m);
 *     - Return F(n) mod m. F(0)=0, F(1)=1. m <= 1 → 0.
 *       Fast doubling (O(log n) modular multiplies); product fits in
 *       uint64 for u32 factors.
 *   uint32_t __gj_u32_fib_mod_u  (alias)
 *   __libcgj_batch3517_marker = "libcgj-batch3517"
 *
 * Exclusive continuum CREATE-ONLY (3511-3520). Unique gj_u32_fib_mod_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3517_marker[] = "libcgj-batch3517";

/* ---- freestanding helpers ---------------------------------------------- */

/* (a * b) mod m for m > 1; product fits in uint64. */
static uint32_t
b3517_mulmod(uint32_t u32A, uint32_t u32B, uint32_t u32M)
{
	return (uint32_t)(((uint64_t)u32A * (uint64_t)u32B) % (uint64_t)u32M);
}

/* (a + b) mod m. */
static uint32_t
b3517_addmod(uint32_t u32A, uint32_t u32B, uint32_t u32M)
{
	return (uint32_t)(((uint64_t)u32A + (uint64_t)u32B) % (uint64_t)u32M);
}

/* (a - b) mod m for a, b in [0, m). */
static uint32_t
b3517_submod(uint32_t u32A, uint32_t u32B, uint32_t u32M)
{
	if (u32A >= u32B) {
		return u32A - u32B;
	}
	return u32M - (u32B - u32A);
}

/*
 * Fast Fibonacci doubling. Walks bits of n MSB→LSB starting from
 * (F(0), F(1)) = (0, 1). On exit *pF = F(n) mod m.
 *
 * Double step (k → 2k):
 *   F(2k)   = F(k) * (2*F(k+1) - F(k))
 *   F(2k+1) = F(k)^2 + F(k+1)^2
 * Odd bit then advances (F(2k), F(2k+1)) → (F(2k+1), F(2k)+F(2k+1)).
 */
static uint32_t
b3517_fib(uint32_t u32N, uint32_t u32M)
{
	uint32_t u32F = 0u; /* F(k) */
	uint32_t u32G = 1u; /* F(k+1) */
	uint32_t u32Bit;

	if (u32N == 0u) {
		return 0u;
	}

	u32Bit = 31u;
	while (u32Bit > 0u && ((u32N >> u32Bit) & 1u) == 0u) {
		u32Bit--;
	}

	for (;;) {
		uint32_t u32F2;
		uint32_t u32G2;
		uint32_t u32TwoG;
		uint32_t u32Diff;

		u32TwoG = b3517_addmod(u32G, u32G, u32M);
		u32Diff = b3517_submod(u32TwoG, u32F, u32M);
		u32F2 = b3517_mulmod(u32F, u32Diff, u32M);
		u32G2 = b3517_addmod(b3517_mulmod(u32F, u32F, u32M),
		                     b3517_mulmod(u32G, u32G, u32M),
		                     u32M);

		if (((u32N >> u32Bit) & 1u) != 0u) {
			u32F = u32G2;
			u32G = b3517_addmod(u32F2, u32G2, u32M);
		} else {
			u32F = u32F2;
			u32G = u32G2;
		}

		if (u32Bit == 0u) {
			break;
		}
		u32Bit--;
	}

	return u32F;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_fib_mod_u - Fibonacci F(n) modulo m.
 *
 * n: index (F(0)=0, F(1)=1)
 * m: modulus
 *
 * Returns F(n) mod m in [0, m) when m > 1; 0 when m <= 1.
 * No parent wires.
 */
uint32_t
gj_u32_fib_mod_u(uint32_t u32N, uint32_t u32M)
{
	(void)NULL;

	if (u32M <= 1u) {
		return 0u;
	}
	return b3517_fib(u32N, u32M);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_fib_mod_u(uint32_t u32N, uint32_t u32M)
    __attribute__((alias("gj_u32_fib_mod_u")));
