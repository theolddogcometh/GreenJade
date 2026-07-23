/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2404: uint32_t rotate right by n mod 32.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_rotr_n_u(uint32_t x, unsigned n);
 *     - Rotate x right by (n % 32) bits. n == 0 or multiple of 32
 *       returns x unchanged (no shift UB). Pure integer; no builtins.
 *   uint32_t __gj_u32_rotr_n_u  (alias)
 *   __libcgj_batch2404_marker = "libcgj-batch2404"
 *
 * Milestone 2410 exclusive shift/rotate wave (2401-2410). Unique
 * gj_u32_rotr_n_u surface only; no multi-def. Pair with
 * gj_u32_rotl_n_u (batch2403).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2404_marker[] = "libcgj-batch2404";

/* ---- freestanding helpers ---------------------------------------------- */

/* Rotate u32X right by (uN % 32). Zero mod returns u32X. */
static uint32_t
b2404_rotr(uint32_t u32X, unsigned uN)
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
 * gj_u32_rotr_n_u - rotate a 32-bit word right by n mod 32.
 *
 * x: value to rotate
 * n: rotate count; reduced mod 32 (0 and multiples of 32 are no-ops)
 *
 * Returns the rotated value. Does not call libc.
 */
uint32_t
gj_u32_rotr_n_u(uint32_t u32X, unsigned uN)
{
	(void)NULL;
	return b2404_rotr(u32X, uN);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_rotr_n_u(uint32_t u32X, unsigned uN)
    __attribute__((alias("gj_u32_rotr_n_u")));
