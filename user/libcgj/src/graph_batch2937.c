/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2937: uint32 modular sub (a-b) mod m (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_sub_mod_u(uint32_t a, uint32_t b, uint32_t m);
 *     - (a - b) mod m. m==0 -> 0 (invalid modulus). m==1 -> 0.
 *       Result always in [0, m) when m > 0.
 *   uint32_t __gj_u32_sub_mod_u  (alias)
 *   __libcgj_batch2937_marker = "libcgj-batch2937"
 *
 * Pack/mod exclusive wave (2931-2940). Distinct from wrap sub surfaces
 * — unique modular-with-m _u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2937_marker[] = "libcgj-batch2937";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * (a - b) mod m. m must be non-zero. Reduces a,b mod m first; borrow
 * handled as m - (b - a) when a < b.
 */
static uint32_t
b2937_submod(uint32_t u32A, uint32_t u32B, uint32_t u32M)
{
	if (u32M == 0u) {
		return 0u;
	}
	if (u32M == 1u) {
		return 0u;
	}

	u32A %= u32M;
	u32B %= u32M;
	if (u32A >= u32B) {
		return u32A - u32B;
	}
	return u32M - (u32B - u32A);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_sub_mod_u - (a - b) mod m.
 *
 * a, b: operands (any uint32_t; reduced mod m when m > 0)
 * m:    modulus; m==0 yields 0
 *
 * Returns (a - b) mod m in [0, m), or 0 when m is 0 or 1.
 */
uint32_t
gj_u32_sub_mod_u(uint32_t u32A, uint32_t u32B, uint32_t u32M)
{
	(void)NULL;
	return b2937_submod(u32A, u32B, u32M);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_sub_mod_u(uint32_t u32A, uint32_t u32B, uint32_t u32M)
    __attribute__((alias("gj_u32_sub_mod_u")));
