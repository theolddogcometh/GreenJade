/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2741: rotate-left a uint32_t by n bits (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_rotl_u32_u(uint32_t x, unsigned n);
 *     - Circular left rotate of x by (n mod 32) bit positions.
 *       n is reduced with n &= 31 so any unsigned rotate count is defined
 *       (including 0, which returns x unchanged).
 *   uint32_t __gj_rotl_u32_u  (alias)
 *   __libcgj_batch2741_marker = "libcgj-batch2741"
 *
 * Bitops exclusive wave (2741-2750). Distinct from gj_rotl32 (batch771)
 * and gj_rotl_u64 (batch565) — unique gj_rotl_u32_u surface only; no
 * multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2741_marker[] = "libcgj-batch2741";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2741_rotl(uint32_t u32X, unsigned uN)
{
	uN &= 31u;
	if (uN == 0u) {
		return u32X;
	}
	return (u32X << uN) | (u32X >> (32u - uN));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rotl_u32_u - left-rotate a 32-bit word.
 *
 * x: value to rotate
 * n: rotate count (mod 32 via n &= 31)
 *
 * Zero-count branch avoids a C undefined 32-bit shift of 32.
 */
uint32_t
gj_rotl_u32_u(uint32_t x, unsigned n)
{
	(void)NULL;
	return b2741_rotl(x, n);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rotl_u32_u(uint32_t x, unsigned n)
    __attribute__((alias("gj_rotl_u32_u")));
