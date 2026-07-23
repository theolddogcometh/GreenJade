/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3861: wrapping uint64_t multiply-subtract (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_mul_sub_u(uint64_t a, uint64_t b, uint64_t c);
 *     - Return (a * b) - c with modular wrap (mod 2^64). Plain
 *       unsigned wrap; not saturating. No __int128.
 *   uint64_t __gj_u64_mul_sub_u  (alias)
 *   __libcgj_batch3861_marker = "libcgj-batch3861"
 *
 * Exclusive continuum CREATE-ONLY (3861-3870: u64_mul_sub_u,
 * u32_madd_sat_u, u64_madd_sat_u, u32_msub_sat_u, u64_msub_sat_u,
 * u32_abs_to_i32_u, i32_to_u32_sat_u, i64_to_u64_sat_u,
 * u32_to_i32_sat_u, batch_id_3870). Unique gj_u64_mul_sub_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3861_marker[] = "libcgj-batch3861";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Portable modular wrap mul-sub: (a * b) - c mod 2^64.
 * C unsigned 64-bit mul/sub are defined to wrap; no wide type needed.
 */
static uint64_t
b3861_mul_sub(uint64_t u64A, uint64_t u64B, uint64_t u64C)
{
	return (u64A * u64B) - u64C;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_mul_sub_u - modular wrap multiply-subtract of three uint64_t.
 *
 * a, b: factors
 * c:    subtrahend applied after the product
 *
 * Returns ((a * b) - c) mod 2^64. Software path only: no __int128, no
 * builtins. Does not call libc. No parent wires.
 */
uint64_t
gj_u64_mul_sub_u(uint64_t u64A, uint64_t u64B, uint64_t u64C)
{
	(void)NULL;
	return b3861_mul_sub(u64A, u64B, u64C);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_mul_sub_u(uint64_t u64A, uint64_t u64B, uint64_t u64C)
    __attribute__((alias("gj_u64_mul_sub_u")));
