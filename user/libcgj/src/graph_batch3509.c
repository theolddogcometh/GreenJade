/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3509: modular mul on uint32_t (_u surface).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_mod_mul_u(uint32_t a, uint32_t b, uint32_t m);
 *     — compute (a * b) mod m via binary (Russian-peasant) modular
 *       multiply. Precondition m != 0; if m == 0 returns 0 (soft guard).
 *       Portable: no __int128 / no libgcc __umodti3.
 *   uint32_t __gj_u32_mod_mul_u  (alias)
 *   __libcgj_batch3509_marker = "libcgj-batch3509"
 *
 * Post-3500 modular/number-theory exclusive wave (3501–3510). Unique
 * gj_u32_mod_mul_u surface; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3509_marker[] = "libcgj-batch3509";

/* ---- freestanding helpers ---------------------------------------------- */

/* (a + b) mod m with 0 <= a,b < m (so a+b < 2m). m > 0. */
static uint32_t
b3509_addmod(uint32_t u32A, uint32_t u32B, uint32_t u32M)
{
	uint32_t u32S = u32M - u32A;

	if (u32B >= u32S) {
		return u32B - u32S;
	}
	return u32A + u32B;
}

/*
 * (a * b) mod m for m > 0 — pure uint32, no 128-bit helpers.
 * Russian-peasant multiply with modular doubling/adding.
 * When m == 0, returns 0 (soft guard).
 */
static uint32_t
b3509_mod_mul(uint32_t u32A, uint32_t u32B, uint32_t u32M)
{
	uint32_t u32R = 0u;

	if (u32M == 0u) {
		return 0u;
	}
	u32A %= u32M;
	u32B %= u32M;
	while (u32B != 0u) {
		if ((u32B & 1u) != 0u) {
			u32R = b3509_addmod(u32R, u32A, u32M);
		}
		u32A = b3509_addmod(u32A, u32A, u32M);
		u32B >>= 1;
	}
	return u32R;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_mod_mul_u — (a * b) mod m for uint32_t.
 *
 * a, b: factors; m: modulus (0 → 0 soft guard)
 */
uint32_t
gj_u32_mod_mul_u(uint32_t u32A, uint32_t u32B, uint32_t u32M)
{
	(void)NULL;
	return b3509_mod_mul(u32A, u32B, u32M);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_mod_mul_u(uint32_t u32A, uint32_t u32B, uint32_t u32M)
    __attribute__((alias("gj_u32_mod_mul_u")));
