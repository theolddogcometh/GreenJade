/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4112: uint32_t rotate right by n & 31.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_ror_n_u2(uint32_t x, unsigned n);
 *     - Rotate x right by (n & 31) bits. n == 0 or multiple of 32
 *       returns x unchanged (no shift UB). Pure integer; no builtins.
 *   uint32_t __gj_u32_ror_n_u2  (alias)
 *   __libcgj_batch4112_marker = "libcgj-batch4112"
 *
 * Exclusive continuum CREATE-ONLY (4111-4120). Distinct from
 * gj_u32_rotr_n_u (batch2404) and gj_u32_rol_n_u (batch4111) —
 * unique u32 ror surface in this exclusive range. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4112_marker[] = "libcgj-batch4112";

/* ---- freestanding helpers ---------------------------------------------- */

/* Rotate u32X right by (uN & 31). Zero count returns u32X. */
static uint32_t
b4112_ror(uint32_t u32X, unsigned uN)
{
	unsigned uR;

	uR = uN & 31u;
	if (uR == 0u) {
		return u32X;
	}
	return (u32X >> uR) | (u32X << (32u - uR));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_ror_n_u2 - rotate a 32-bit word right by n & 31.
 *
 * x: value to rotate
 * n: rotate count; reduced with n & 31 (0 and multiples of 32 are no-ops)
 *
 * Returns the rotated value. Does not call libc. No parent wires.
 */
uint32_t
gj_u32_ror_n_u2(uint32_t u32X, unsigned uN)
{
	(void)NULL;
	return b4112_ror(u32X, uN);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_ror_n_u2(uint32_t u32X, unsigned uN)
    __attribute__((alias("gj_u32_ror_n_u2")));
