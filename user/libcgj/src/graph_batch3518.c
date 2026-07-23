/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3518: Fibonacci F(n) mod m for uint64_t.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_fib_mod_u(uint64_t n, uint64_t m);
 *     - Return F(n) mod m. F(0)=0, F(1)=1. m <= 1 → 0.
 *       Fast doubling with Russian-peasant modular multiply (no
 *       __int128). O(log n) modular multiplies.
 *   uint64_t __gj_u64_fib_mod_u  (alias)
 *   __libcgj_batch3518_marker = "libcgj-batch3518"
 *
 * Exclusive continuum CREATE-ONLY (3511-3520). Unique gj_u64_fib_mod_u
 * surface only; no multi-def. Self-contained (no parent wires to
 * gj_u64_mod_mul_u / gj_u32_fib_mod_u). No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3518_marker[] = "libcgj-batch3518";

/* ---- freestanding helpers ---------------------------------------------- */

/* (a + b) mod m for a, b in [0, m). m > 1. */
static uint64_t
b3518_addmod(uint64_t u64A, uint64_t u64B, uint64_t u64M)
{
	if (u64B >= (u64M - u64A)) {
		return u64A - (u64M - u64B);
	}
	return u64A + u64B;
}

/* (a - b) mod m for a, b in [0, m). */
static uint64_t
b3518_submod(uint64_t u64A, uint64_t u64B, uint64_t u64M)
{
	if (u64A >= u64B) {
		return u64A - u64B;
	}
	return u64M - (u64B - u64A);
}

/* (a * b) mod m; a, b reduced. No __int128 — double-and-add. */
static uint64_t
b3518_mulmod(uint64_t u64A, uint64_t u64B, uint64_t u64M)
{
	uint64_t u64R;

	u64A %= u64M;
	u64B %= u64M;
	u64R = 0u;
	while (u64B != 0u) {
		if ((u64B & 1u) != 0u) {
			u64R = b3518_addmod(u64R, u64A, u64M);
		}
		u64A = b3518_addmod(u64A, u64A, u64M);
		u64B >>= 1;
	}
	return u64R;
}

/* Fast Fibonacci doubling; returns F(n) mod m (m > 1). */
static uint64_t
b3518_fib(uint64_t u64N, uint64_t u64M)
{
	uint64_t u64F = 0u;
	uint64_t u64G = 1u;
	uint32_t uBit;

	if (u64N == 0u) {
		return 0u;
	}

	uBit = 63u;
	while (uBit > 0u && ((u64N >> uBit) & 1u) == 0u) {
		uBit--;
	}

	for (;;) {
		uint64_t u64F2;
		uint64_t u64G2;
		uint64_t u64TwoG;
		uint64_t u64Diff;

		u64TwoG = b3518_addmod(u64G, u64G, u64M);
		u64Diff = b3518_submod(u64TwoG, u64F, u64M);
		u64F2 = b3518_mulmod(u64F, u64Diff, u64M);
		u64G2 = b3518_addmod(b3518_mulmod(u64F, u64F, u64M),
		                     b3518_mulmod(u64G, u64G, u64M),
		                     u64M);

		if (((u64N >> uBit) & 1u) != 0u) {
			u64F = u64G2;
			u64G = b3518_addmod(u64F2, u64G2, u64M);
		} else {
			u64F = u64F2;
			u64G = u64G2;
		}

		if (uBit == 0u) {
			break;
		}
		uBit--;
	}

	return u64F;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_fib_mod_u - Fibonacci F(n) modulo m (64-bit).
 *
 * n: index (F(0)=0, F(1)=1)
 * m: modulus
 *
 * Returns F(n) mod m in [0, m) when m > 1; 0 when m <= 1.
 * No parent wires. No __int128.
 */
uint64_t
gj_u64_fib_mod_u(uint64_t u64N, uint64_t u64M)
{
	(void)NULL;

	if (u64M <= 1u) {
		return 0u;
	}
	return b3518_fib(u64N, u64M);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_fib_mod_u(uint64_t u64N, uint64_t u64M)
    __attribute__((alias("gj_u64_fib_mod_u")));
