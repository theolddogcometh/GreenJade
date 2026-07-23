/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3858: unsigned 64-bit multiply-add (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_mul_add_u(uint64_t a, uint64_t b, uint64_t c);
 *     - Return (a * b) + c with modular wrap (mod 2^64).
 *       Plain unsigned wrap; not saturating. No __int128.
 *   uint64_t __gj_u64_mul_add_u  (alias)
 *   __libcgj_batch3858_marker = "libcgj-batch3858"
 *
 * Exclusive continuum CREATE-ONLY (3851-3860). Distinct from
 * gj_u64_mul_wrap_soft (batch2316) and gj_u32_mul_add_u (batch3857) —
 * unique gj_u64_mul_add_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3858_marker[] = "libcgj-batch3858";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Portable modular wrap mul-add mod 2^64.
 * C unsigned 64-bit mul/add is defined to wrap; no wide type needed.
 */
static uint64_t
b3858_mul_add(uint64_t u64A, uint64_t u64B, uint64_t u64C)
{
	return (u64A * u64B) + u64C;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_mul_add_u - modular wrap multiply-add of three uint64_t values.
 *
 * a, b: factors
 * c: addend
 *
 * Returns ((a * b) + c) mod 2^64. Software path only: no __int128,
 * no __uint128_t. No parent wires.
 */
uint64_t
gj_u64_mul_add_u(uint64_t u64A, uint64_t u64B, uint64_t u64C)
{
	(void)NULL;
	return b3858_mul_add(u64A, u64B, u64C);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_mul_add_u(uint64_t u64A, uint64_t u64B, uint64_t u64C)
    __attribute__((alias("gj_u64_mul_add_u")));
