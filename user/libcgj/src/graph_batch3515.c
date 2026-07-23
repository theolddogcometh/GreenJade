/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3515: binomial coefficient modulo m.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_binom_mod_u(uint32_t n, uint32_t k, uint32_t m);
 *     - Return C(n, k) mod m. m <= 1 → 0; k > n → 0; C(n,0)=C(n,n)=1.
 *       Multiplicative formula with modular inverses of 1..k. Requires
 *       each denominator to be invertible mod m (always true for prime
 *       m when k < m; for composite m returns 0 if an inverse is
 *       missing). Self-contained; no parent wires.
 *   uint32_t __gj_u32_binom_mod_u  (alias)
 *   __libcgj_batch3515_marker = "libcgj-batch3515"
 *
 * Exclusive continuum CREATE-ONLY (3511-3520). Unique gj_u32_binom_mod_u
 * surface only; no multi-def. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3515_marker[] = "libcgj-batch3515";

/* ---- freestanding helpers ---------------------------------------------- */

/* (a * b) mod m for m > 1; product fits in uint64. */
static uint32_t
b3515_mulmod(uint32_t u32A, uint32_t u32B, uint32_t u32M)
{
	return (uint32_t)(((uint64_t)u32A * (uint64_t)u32B) % (uint64_t)u32M);
}

/* Modular inverse; 0 if none (m > 1). */
static uint32_t
b3515_modinv(uint32_t u32A, uint32_t u32M)
{
	uint32_t u32T = 0u;
	uint32_t u32NewT = 1u;
	uint32_t u32R = u32M;
	uint32_t u32NewR;

	u32A %= u32M;
	if (u32A == 0u) {
		return 0u;
	}
	u32NewR = u32A;

	while (u32NewR != 0u) {
		uint32_t u32Q = u32R / u32NewR;
		uint32_t u32Tmp;
		uint32_t u32QN;
		uint32_t u32NextT;

		u32Tmp = u32R - u32Q * u32NewR;
		u32R = u32NewR;
		u32NewR = u32Tmp;

		u32QN = (uint32_t)(((uint64_t)u32Q * (uint64_t)u32NewT) %
		                   (uint64_t)u32M);
		if (u32T >= u32QN) {
			u32NextT = u32T - u32QN;
		} else {
			u32NextT = u32M - (u32QN - u32T);
		}
		u32T = u32NewT;
		u32NewT = u32NextT;
	}

	if (u32R != 1u) {
		return 0u;
	}
	return u32T % u32M;
}

/*
 * C(n, k) mod m via product:
 *   C = Π_{i=1..k} (n - k + i) / i
 * Multiplies (n-k+i) and inv(i) mod m. k already min(k, n-k).
 */
static uint32_t
b3515_binom_mod(uint32_t u32N, uint32_t u32K, uint32_t u32M)
{
	uint32_t u32R;
	uint32_t u32I;

	u32R = 1u;
	for (u32I = 1u; u32I <= u32K; u32I++) {
		uint32_t u32Num;
		uint32_t u32Inv;

		/* n - k + i  (k <= n/2 so this stays <= n) */
		u32Num = (u32N - u32K) + u32I;
		u32R = b3515_mulmod(u32R, u32Num % u32M, u32M);
		u32Inv = b3515_modinv(u32I, u32M);
		if (u32Inv == 0u) {
			return 0u;
		}
		u32R = b3515_mulmod(u32R, u32Inv, u32M);
	}
	return u32R;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_binom_mod_u - binomial C(n, k) modulo m.
 *
 * n, k: binomial arguments
 * m:    modulus
 *
 * Returns C(n,k) mod m, or 0 on invalid modulus / missing inverse.
 * No parent wires.
 */
uint32_t
gj_u32_binom_mod_u(uint32_t u32N, uint32_t u32K, uint32_t u32M)
{
	(void)NULL;

	if (u32M <= 1u) {
		return 0u;
	}
	if (u32K > u32N) {
		return 0u;
	}
	if (u32K == 0u || u32K == u32N) {
		return 1u % u32M;
	}
	/* C(n,k) = C(n, n-k); take the smaller side. */
	if (u32K > (u32N - u32K)) {
		u32K = u32N - u32K;
	}
	return b3515_binom_mod(u32N, u32K, u32M);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_binom_mod_u(uint32_t u32N, uint32_t u32K, uint32_t u32M)
    __attribute__((alias("gj_u32_binom_mod_u")));
