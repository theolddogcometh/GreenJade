/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2743: rotate-left a uint64_t by n bits (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_rotl_u64_u(uint64_t x, unsigned n);
 *     - Circular left rotate of x by (n mod 64) bit positions.
 *       n is reduced with n &= 63 so any unsigned rotate count is defined
 *       (including 0, which returns x unchanged).
 *   uint64_t __gj_rotl_u64_u  (alias)
 *   __libcgj_batch2743_marker = "libcgj-batch2743"
 *
 * Bitops exclusive wave (2741-2750). Distinct from gj_rotl_u64
 * (batch565) and gj_rotl64 (batch369) — unique gj_rotl_u64_u surface
 * only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2743_marker[] = "libcgj-batch2743";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b2743_rotl(uint64_t u64X, unsigned uN)
{
	uN &= 63u;
	if (uN == 0u) {
		return u64X;
	}
	return (u64X << uN) | (u64X >> (64u - uN));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rotl_u64_u - left-rotate a 64-bit word.
 *
 * x: value to rotate
 * n: rotate count (mod 64 via n &= 63)
 *
 * Zero-count branch avoids a C undefined 64-bit shift of 64.
 */
uint64_t
gj_rotl_u64_u(uint64_t x, unsigned n)
{
	(void)NULL;
	return b2743_rotl(x, n);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_rotl_u64_u(uint64_t x, unsigned n)
    __attribute__((alias("gj_rotl_u64_u")));
