/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2938: uint32 modular multiply (a*b) mod m (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_mul_mod_u(uint32_t a, uint32_t b, uint32_t m);
 *     - (a * b) mod m. m==0 -> 0 (invalid modulus). m==1 -> 0.
 *       Binary modular multiply (no 64-bit product reliance, no __int128).
 *   uint32_t __gj_u32_mul_mod_u  (alias)
 *   __libcgj_batch2938_marker = "libcgj-batch2938"
 *
 * Pack/mod exclusive wave (2931-2940). Distinct from gj_mul_mod_u32
 * (batch841) — unique gj_u32_mul_mod_u surface only; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2938_marker[] = "libcgj-batch2938";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * (a * b) mod m via Russian-peasant / binary modular multiply.
 * Avoids intermediate product overflow without wider types.
 */
static uint32_t
b2938_mulmod(uint32_t u32A, uint32_t u32B, uint32_t u32M)
{
	uint32_t u32R;

	if (u32M == 0u) {
		return 0u;
	}
	if (u32M == 1u) {
		return 0u;
	}

	u32A %= u32M;
	u32B %= u32M;
	u32R = 0u;
	while (u32B != 0u) {
		if ((u32B & 1u) != 0u) {
			uint32_t u32S = u32M - u32R;

			u32R = (u32A >= u32S) ? (u32A - u32S) : (u32R + u32A);
		}
		{
			uint32_t u32S = u32M - u32A;

			u32A = (u32A >= u32S) ? (u32A - u32S) : (u32A + u32A);
		}
		u32B >>= 1;
	}
	return u32R;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_mul_mod_u - (a * b) mod m without __int128 intermediates.
 *
 * a, b: factors (reduced mod m when m > 0)
 * m:    modulus; m==0 yields 0
 *
 * Returns (a * b) mod m in [0, m), or 0 when m is 0 or 1.
 */
uint32_t
gj_u32_mul_mod_u(uint32_t u32A, uint32_t u32B, uint32_t u32M)
{
	(void)NULL;
	return b2938_mulmod(u32A, u32B, u32M);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_mul_mod_u(uint32_t u32A, uint32_t u32B, uint32_t u32M)
    __attribute__((alias("gj_u32_mul_mod_u")));
