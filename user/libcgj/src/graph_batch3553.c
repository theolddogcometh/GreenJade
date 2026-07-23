/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3553: uint32_t rotate right by n mod 32 (ror).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_ror_n_u(uint32_t x, unsigned n);
 *     - Rotate x right by (n % 32) bits. n == 0 or multiple of 32
 *       returns x unchanged (no shift UB). Pure integer; no builtins.
 *   uint32_t __gj_u32_ror_n_u  (alias)
 *   __libcgj_batch3553_marker = "libcgj-batch3553"
 *
 * Exclusive continuum CREATE-ONLY (3551-3560). Distinct from
 * gj_u32_rotr_n_u (batch2404) and gj_u32_rol_n_u (batch3551) —
 * unique u32 ror surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3553_marker[] = "libcgj-batch3553";

/* ---- freestanding helpers ---------------------------------------------- */

/* Rotate u32X right by (uN % 32). Zero mod returns u32X. */
static uint32_t
b3553_ror(uint32_t u32X, unsigned uN)
{
	unsigned uR;

	uR = uN % 32u;
	if (uR == 0u) {
		return u32X;
	}
	return (u32X >> uR) | (u32X << (32u - uR));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_ror_n_u - rotate a 32-bit word right by n mod 32.
 *
 * x: value to rotate
 * n: rotate count; reduced mod 32 (0 and multiples of 32 are no-ops)
 *
 * Returns the rotated value. Does not call libc. No parent wires.
 */
uint32_t
gj_u32_ror_n_u(uint32_t u32X, unsigned uN)
{
	(void)NULL;
	return b3553_ror(u32X, uN);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_ror_n_u(uint32_t u32X, unsigned uN)
    __attribute__((alias("gj_u32_ror_n_u")));
