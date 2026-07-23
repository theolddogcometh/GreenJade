/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3512: modular multiplicative inverse uint32_t.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_mod_inv_u(uint32_t a, uint32_t m);
 *     - Return x in [0, m) with (a * x) ≡ 1 (mod m), or 0 if no
 *       inverse exists (m <= 1, or gcd(a mod m, m) != 1). Soft
 *       unsigned extended Euclid; no signed overflow.
 *   uint32_t __gj_u32_mod_inv_u  (alias)
 *   __libcgj_batch3512_marker = "libcgj-batch3512"
 *
 * Exclusive continuum CREATE-ONLY (3511-3520). Unique gj_u32_mod_inv_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3512_marker[] = "libcgj-batch3512";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Modular inverse of a modulo m (m > 1). Soft Bézout coefficients
 * tracked in [0, m). Returns inv in [0, m), or 0 if gcd != 1.
 * Note: the true inverse is never 0 for m > 1, so 0 is a safe sentinel.
 */
static uint32_t
b3512_modinv(uint32_t u32A, uint32_t u32M)
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

		/* next_t ≡ t - q*newt (mod m) */
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
 * gj_u32_mod_inv_u - modular multiplicative inverse of a mod m.
 *
 * a: value to invert
 * m: modulus
 *
 * Returns x in [0, m) with a*x ≡ 1 (mod m), or 0 if m <= 1 or gcd != 1.
 * No parent wires.
 */
uint32_t
gj_u32_mod_inv_u(uint32_t u32A, uint32_t u32M)
{
	(void)NULL;

	if (u32M <= 1u) {
		return 0u;
	}
	return b3512_modinv(u32A, u32M);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_mod_inv_u(uint32_t u32A, uint32_t u32M)
    __attribute__((alias("gj_u32_mod_inv_u")));
