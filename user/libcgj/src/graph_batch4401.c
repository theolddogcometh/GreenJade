/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4401: high 32 bits of uint32×uint32 product
 * (exclusive _u2; rename on collision with batch3007 gj_u32_mul_hi_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_mul_hi_u2(uint32_t a, uint32_t b);
 *     - Return floor((a * b) / 2^32), i.e. the high half of the
 *       full 64-bit unsigned product. Uses a widening uint64_t multiply
 *       (no __int128).
 *   uint32_t __gj_u32_mul_hi_u2  (alias)
 *   __libcgj_batch4401_marker = "libcgj-batch4401"
 *
 * Exclusive continuum CREATE-ONLY (4401-4410: u32_mul_hi_u2,
 * u64_mul_hi_soft_u, u32_mul_sat_u, u64_mul_sat_u, u32_div_ceil_u2,
 * u64_div_ceil_u3, u32_div_floor_u2, u32_mod_safe_u, u64_mod_safe_u,
 * batch_id_4410). Distinct from gj_u32_mul_hi_u (batch3007) and
 * gj_mul_hi_u64 (batch568) — unique gj_u32_mul_hi_u2 surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4401_marker[] = "libcgj-batch4401";

/* ---- freestanding helpers ---------------------------------------------- */

/* High 32 bits of a*b (full 64-bit product). */
static uint32_t
b4401_mul_hi(uint32_t u32A, uint32_t u32B)
{
	uint64_t u64P;

	u64P = (uint64_t)u32A * (uint64_t)u32B;
	return (uint32_t)(u64P >> 32);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_mul_hi_u2 - high 32 bits of a 64-bit unsigned product.
 *
 * a, b: factors
 * Returns floor((a * b) / 2^32). No parent wires. No __int128.
 */
uint32_t
gj_u32_mul_hi_u2(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b4401_mul_hi(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_mul_hi_u2(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_u32_mul_hi_u2")));
