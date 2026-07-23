/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4114: uint64_t rotate right by n & 63.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_ror_n_u2(uint64_t x, unsigned n);
 *     - Rotate x right by (n & 63) bits. n == 0 or multiple of 64
 *       returns x unchanged (no shift UB). Pure integer; no builtins.
 *   uint64_t __gj_u64_ror_n_u2  (alias)
 *   __libcgj_batch4114_marker = "libcgj-batch4114"
 *
 * Exclusive continuum CREATE-ONLY (4111-4120). Distinct from
 * gj_u64_rotr_n_u (batch2402) and gj_u64_rol_n_u (batch4113) —
 * unique u64 ror surface in this exclusive range. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4114_marker[] = "libcgj-batch4114";

/* ---- freestanding helpers ---------------------------------------------- */

/* Rotate u64X right by (uN & 63). Zero count returns u64X. */
static uint64_t
b4114_ror(uint64_t u64X, unsigned uN)
{
	unsigned uR;

	uR = uN & 63u;
	if (uR == 0u) {
		return u64X;
	}
	return (u64X >> uR) | (u64X << (64u - uR));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_ror_n_u2 - rotate a 64-bit word right by n & 63.
 *
 * x: value to rotate
 * n: rotate count; reduced with n & 63 (0 and multiples of 64 are no-ops)
 *
 * Returns the rotated value. Does not call libc. No parent wires.
 */
uint64_t
gj_u64_ror_n_u2(uint64_t u64X, unsigned uN)
{
	(void)NULL;
	return b4114_ror(u64X, uN);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_ror_n_u2(uint64_t u64X, unsigned uN)
    __attribute__((alias("gj_u64_ror_n_u2")));
