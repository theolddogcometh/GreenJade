/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2316: portable wrapping uint64_t multiply
 * (post-2310 arith exclusive surface). Soft path only - no int128,
 * no compiler mul-overflow builtins.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_mul_wrap_soft(uint64_t a, uint64_t b);
 *     - Return a * b with modular wrap (mod 2^64). Plain unsigned
 *       wrap via portable C; not saturating. No __int128.
 *   uint64_t __gj_u64_mul_wrap_soft  (alias)
 *   __libcgj_batch2316_marker = "libcgj-batch2316"
 *
 * Post-2310 wrap/ovf exclusive wave (2311-2320). Distinct from
 * gj_u64_mul_sat_soft (batch2033) and gj_u32_mul_wrap (batch2313) -
 * unique gj_u64_mul_wrap_soft surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2316_marker[] = "libcgj-batch2316";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Portable modular wrap mul mod 2^64.
 * C unsigned 64-bit multiply is defined to wrap; no wide type needed.
 */
static uint64_t
b2316_mul(uint64_t u64A, uint64_t u64B)
{
	return u64A * u64B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_mul_wrap_soft - portable modular wrap multiply of two uint64_t.
 *
 * a, b: factors
 *
 * Returns (a * b) mod 2^64. Software path only: no __int128, no
 * __uint128_t, no __builtin_mul_overflow. Does not call libc.
 */
uint64_t
gj_u64_mul_wrap_soft(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b2316_mul(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_mul_wrap_soft(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_mul_wrap_soft")));
