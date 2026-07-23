/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3613: high half of uint32 widening multiply (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_widening_mul_hi_u(uint32_t a, uint32_t b);
 *     - Return floor((a * b) / 2^32), i.e. the high half of the full
 *       64-bit unsigned product formed by widening both factors to
 *       uint64_t (no __int128).
 *   uint32_t __gj_u32_widening_mul_hi_u  (alias)
 *   __libcgj_batch3613_marker = "libcgj-batch3613"
 *
 * Exclusive continuum CREATE-ONLY wave (3611-3620). Unique
 * gj_u32_widening_mul_hi_u surface only; no multi-def. Distinct from
 * gj_u32_mul_hi_u (batch3007) and gj_u32_widening_mul_lo_u (batch3612).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3613_marker[] = "libcgj-batch3613";

/* ---- freestanding helpers ---------------------------------------------- */

/* High 32 bits of a*b (full 64-bit product). */
static uint32_t
b3613_mul_hi(uint32_t u32A, uint32_t u32B)
{
	uint64_t u64P;

	u64P = (uint64_t)u32A * (uint64_t)u32B;
	return (uint32_t)(u64P >> 32);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_widening_mul_hi_u - high 32 bits of a 64-bit unsigned product.
 *
 * a, b: factors
 * Returns floor((a * b) / 2^32). No parent wires. No __int128.
 */
uint32_t
gj_u32_widening_mul_hi_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b3613_mul_hi(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_widening_mul_hi_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_u32_widening_mul_hi_u")));
