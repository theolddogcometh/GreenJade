/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch841: uint32 modular multiply (a*b) mod m.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mul_mod_u32(uint32_t a, uint32_t b, uint32_t m);
 *     — (a * b) mod m. m==0 → 0 (invalid modulus). m==1 → 0.
 *   uint32_t __gj_mul_mod_u32  (alias)
 *   __libcgj_batch841_marker = "libcgj-batch841"
 *
 * Does not redefine gj_modpow_u64 / internal mulmod helpers — unique public symbol.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch841_marker[] = "libcgj-batch841";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mul_mod_u32 — (a * b) mod m without 64-bit intermediate reliance.
 *
 * Uses binary modular multiply so freestanding builds never need a 64x64
 * high product helper. a and b are reduced mod m first when m > 0.
 */
uint32_t
gj_mul_mod_u32(uint32_t u32A, uint32_t u32B, uint32_t u32M)
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

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mul_mod_u32(uint32_t u32A, uint32_t u32B, uint32_t u32M)
    __attribute__((alias("gj_mul_mod_u32")));
