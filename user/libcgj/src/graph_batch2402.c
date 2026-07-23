/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2402: uint64_t rotate right by n mod 64.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_rotr_n_u(uint64_t x, unsigned n);
 *     - Rotate x right by (n % 64) bits. n == 0 or multiple of 64
 *       returns x unchanged (no shift UB). Pure integer; no builtins.
 *   uint64_t __gj_u64_rotr_n_u  (alias)
 *   __libcgj_batch2402_marker = "libcgj-batch2402"
 *
 * Milestone 2410 exclusive shift/rotate wave (2401-2410). Unique
 * gj_u64_rotr_n_u surface only; no multi-def. Pair with
 * gj_u64_rotl_n_u (batch2401).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2402_marker[] = "libcgj-batch2402";

/* ---- freestanding helpers ---------------------------------------------- */

/* Rotate u64X right by (uN % 64). Zero mod returns u64X. */
static uint64_t
b2402_rotr(uint64_t u64X, unsigned uN)
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
 * gj_u64_rotr_n_u - rotate a 64-bit word right by n mod 64.
 *
 * x: value to rotate
 * n: rotate count; reduced mod 64 (0 and multiples of 64 are no-ops)
 *
 * Returns the rotated value. Does not call libc.
 */
uint64_t
gj_u64_rotr_n_u(uint64_t u64X, unsigned uN)
{
	(void)NULL;
	return b2402_rotr(u64X, uN);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_rotr_n_u(uint64_t u64X, unsigned uN)
    __attribute__((alias("gj_u64_rotr_n_u")));
