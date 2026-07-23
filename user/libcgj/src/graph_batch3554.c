/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3554: uint64_t rotate right by n mod 64 (ror).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_ror_n_u(uint64_t x, unsigned n);
 *     - Rotate x right by (n % 64) bits. n == 0 or multiple of 64
 *       returns x unchanged (no shift UB). Pure integer; no builtins.
 *   uint64_t __gj_u64_ror_n_u  (alias)
 *   __libcgj_batch3554_marker = "libcgj-batch3554"
 *
 * Exclusive continuum CREATE-ONLY (3551-3560). Distinct from
 * gj_u64_rotr_n_u (batch2402) and gj_u64_rol_n_u (batch3552) —
 * unique u64 ror surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3554_marker[] = "libcgj-batch3554";

/* ---- freestanding helpers ---------------------------------------------- */

/* Rotate u64X right by (uN % 64). Zero mod returns u64X. */
static uint64_t
b3554_ror(uint64_t u64X, unsigned uN)
{
	unsigned uR;

	uR = uN % 64u;
	if (uR == 0u) {
		return u64X;
	}
	return (u64X >> uR) | (u64X << (64u - uR));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_ror_n_u - rotate a 64-bit word right by n mod 64.
 *
 * x: value to rotate
 * n: rotate count; reduced mod 64 (0 and multiples of 64 are no-ops)
 *
 * Returns the rotated value. Does not call libc. No parent wires.
 */
uint64_t
gj_u64_ror_n_u(uint64_t u64X, unsigned uN)
{
	(void)NULL;
	return b3554_ror(u64X, uN);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_ror_n_u(uint64_t u64X, unsigned uN)
    __attribute__((alias("gj_u64_ror_n_u")));
