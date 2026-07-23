/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4116: uint32_t logical right shift by n & 31.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_shr_arith_u(uint32_t x, unsigned n);
 *     - Logical right shift of x by (n & 31) bits (zero-fill). Despite
 *       the historical "arith" token in the public name, the contract
 *       for this continuum surface is logical SHR on the unsigned word.
 *   uint32_t __gj_u32_shr_arith_u  (alias)
 *   __libcgj_batch4116_marker = "libcgj-batch4116"
 *
 * Exclusive continuum CREATE-ONLY (4111-4120). Distinct from
 * gj_u32_shr_sat (batch2408) and gj_u64_ashr_sat (batch2409) —
 * unique gj_u32_shr_arith_u surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4116_marker[] = "libcgj-batch4116";

/* ---- freestanding helpers ---------------------------------------------- */

/* Logical right shift by (uN & 31). Zero count returns u32X. */
static uint32_t
b4116_shr(uint32_t u32X, unsigned uN)
{
	unsigned uR;

	uR = uN & 31u;
	if (uR == 0u) {
		return u32X;
	}
	return u32X >> uR;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_shr_arith_u - logical right shift of a 32-bit word by n & 31.
 *
 * x: value to shift
 * n: shift count; reduced with n & 31
 *
 * Returns x >> (n & 31) with zero fill. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_u32_shr_arith_u(uint32_t u32X, unsigned uN)
{
	(void)NULL;
	return b4116_shr(u32X, uN);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_shr_arith_u(uint32_t u32X, unsigned uN)
    __attribute__((alias("gj_u32_shr_arith_u")));
