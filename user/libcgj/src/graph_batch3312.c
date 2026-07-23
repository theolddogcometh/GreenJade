/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3312: uint32 modular exponentiation (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_powmod_u(uint32_t a, uint32_t e, uint32_t m);
 *     — compute (a^e) mod m via binary exponentiation.
 *       Precondition m != 0; if m == 0 returns 0 (soft guard).
 *       m == 1 → 0. e == 0 → 1 when m > 1.
 *   uint32_t __gj_u32_powmod_u  (alias)
 *   __libcgj_batch3312_marker = "libcgj-batch3312"
 *
 * CREATE-ONLY exclusive continuum wave (3311–3320). Unique
 * gj_u32_powmod_u surface only; no multi-def. Distinct from
 * gj_pow_mod_u32 (batch842) and gj_pow_mod_u32_u (batch2943). No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3312_marker[] = "libcgj-batch3312";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * (a * b) mod m for m > 1.
 * Product fits in uint64_t when a,b < m <= UINT32_MAX; freestanding-safe
 * (no __int128).
 */
static uint32_t
b3312_mulmod(uint32_t u32A, uint32_t u32B, uint32_t u32M)
{
	return (uint32_t)(((uint64_t)u32A * (uint64_t)u32B) % (uint64_t)u32M);
}

/* (a^e) mod m, m > 1. */
static uint32_t
b3312_powmod(uint32_t u32A, uint32_t u32E, uint32_t u32M)
{
	uint32_t u32R = 1u;

	u32A %= u32M;
	while (u32E != 0u) {
		if ((u32E & 1u) != 0u) {
			u32R = b3312_mulmod(u32R, u32A, u32M);
		}
		u32A = b3312_mulmod(u32A, u32A, u32M);
		u32E >>= 1;
	}
	return u32R;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_powmod_u — portable (a^e) mod m.
 *
 * a: base
 * e: exponent
 * m: modulus (must be non-zero; soft 0 if m == 0)
 *
 * Special cases:
 *   m == 0  → 0  (soft guard; API documents m != 0)
 *   m == 1  → always 0
 *   e == 0  → 1 when m > 1
 */
uint32_t
gj_u32_powmod_u(uint32_t u32A, uint32_t u32E, uint32_t u32M)
{
	(void)NULL;
	if (u32M == 0u || u32M == 1u) {
		return 0u;
	}
	return b3312_powmod(u32A, u32E, u32M);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_powmod_u(uint32_t u32A, uint32_t u32E, uint32_t u32M)
    __attribute__((alias("gj_u32_powmod_u")));
