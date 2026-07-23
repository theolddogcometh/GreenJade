/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3555: uint32_t UB-safe left shift.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_shl_safe_u(uint32_t x, unsigned n);
 *     - Return x << n for n in 0..31. If n >= 32, return 0 (no shift
 *       UB). Pure integer; no builtins.
 *   uint32_t __gj_u32_shl_safe_u  (alias)
 *   __libcgj_batch3555_marker = "libcgj-batch3555"
 *
 * Exclusive continuum CREATE-ONLY (3551-3560). Distinct from
 * gj_u32_shl_sat (batch2407) — unique safe surface only; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3555_marker[] = "libcgj-batch3555";

/* ---- freestanding helpers ---------------------------------------------- */

/* Logical left shift; n >= 32 is a no-UB zero. */
static uint32_t
b3555_shl_safe(uint32_t u32X, unsigned uN)
{
	if (uN >= 32u) {
		return 0u;
	}
	return u32X << uN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_shl_safe_u - UB-safe left shift of a 32-bit word.
 *
 * x: value to shift
 * n: shift count; n >= 32 -> 0
 *
 * Returns x << n for n in 0..31; otherwise 0. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_u32_shl_safe_u(uint32_t u32X, unsigned uN)
{
	(void)NULL;
	return b3555_shl_safe(u32X, uN);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_shl_safe_u(uint32_t u32X, unsigned uN)
    __attribute__((alias("gj_u32_shl_safe_u")));
