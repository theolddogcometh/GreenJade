/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3513: modular division for uint32_t.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_mod_div_u(uint32_t a, uint32_t b, uint32_t m);
 *     - Return (a * inv(b)) mod m, i.e. x with b*x ≡ a (mod m) when
 *       gcd(b, m) == 1. Returns 0 if m <= 1 or b has no inverse mod m.
 *   uint32_t __gj_u32_mod_div_u  (alias)
 *   __libcgj_batch3513_marker = "libcgj-batch3513"
 *
 * Exclusive continuum CREATE-ONLY (3511-3520). Unique gj_u32_mod_div_u
 * surface only; no multi-def. Self-contained (no parent wires to
 * gj_u32_mod_inv_u). No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3513_marker[] = "libcgj-batch3513";

/* ---- freestanding helpers ---------------------------------------------- */

/* (a * b) mod m for m > 1; product fits in uint64. */
static uint32_t
b3513_mulmod(uint32_t u32A, uint32_t u32B, uint32_t u32M)
{
	return (uint32_t)(((uint64_t)u32A * (uint64_t)u32B) % (uint64_t)u32M);
}

/*
 * Modular inverse of b modulo m (m > 1). Soft Bézout in [0, m).
 * Returns inv in [0, m), or 0 if gcd != 1.
 */
static uint32_t
b3513_modinv(uint32_t u32B, uint32_t u32M)
{
	uint32_t u32T = 0u;
	uint32_t u32NewT = 1u;
	uint32_t u32R = u32M;
	uint32_t u32NewR;

	u32B %= u32M;
	if (u32B == 0u) {
		return 0u;
	}
	u32NewR = u32B;

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

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_mod_div_u - modular division a / b mod m.
 *
 * a: numerator
 * b: denominator (must be invertible mod m)
 * m: modulus
 *
 * Returns (a * inv(b)) mod m in [0, m), or 0 if m <= 1 or no inverse.
 * No parent wires.
 */
uint32_t
gj_u32_mod_div_u(uint32_t u32A, uint32_t u32B, uint32_t u32M)
{
	uint32_t u32Inv;

	(void)NULL;

	if (u32M <= 1u) {
		return 0u;
	}
	u32Inv = b3513_modinv(u32B, u32M);
	if (u32Inv == 0u) {
		return 0u;
	}
	return b3513_mulmod(u32A % u32M, u32Inv, u32M);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_mod_div_u(uint32_t u32A, uint32_t u32B, uint32_t u32M)
    __attribute__((alias("gj_u32_mod_div_u")));
