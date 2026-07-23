/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4305: fill-safe uint32_t left shift (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_shl_fill_u(uint32_t x, unsigned n);
 *     - Return x << n for n in 0..31. If n >= 32, return 0 (fill/zero
 *       the result; no shift UB). Pure integer; no builtins.
 *   uint32_t __gj_u32_shl_fill_u  (alias)
 *   __libcgj_batch4305_marker = "libcgj-batch4305"
 *
 * Exclusive continuum CREATE-ONLY (4301-4310). Distinct from
 * gj_u32_shl_checked_u (batch4301) and gj_u32_shl_sat (batch2407) —
 * unique gj_u32_shl_fill_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4305_marker[] = "libcgj-batch4305";

/* ---- freestanding helpers ---------------------------------------------- */

/* Left shift; n >= 32 fills result with 0. */
static uint32_t
b4305_shl_fill(uint32_t u32X, unsigned uN)
{
	if (uN >= 32u) {
		return 0u;
	}
	return u32X << uN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_shl_fill_u - fill-safe left shift of a 32-bit word.
 *
 * x: value to shift
 * n: shift count; n >= 32 -> 0
 *
 * Returns x << n for n in 0..31; otherwise 0. Does not call libc.
 * Self-contained; no parent wires.
 */
uint32_t
gj_u32_shl_fill_u(uint32_t u32X, unsigned uN)
{
	(void)NULL;
	return b4305_shl_fill(u32X, uN);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_shl_fill_u(uint32_t u32X, unsigned uN)
    __attribute__((alias("gj_u32_shl_fill_u")));
