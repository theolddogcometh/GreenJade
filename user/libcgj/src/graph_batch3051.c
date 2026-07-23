/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3051: portable uint32 multiply-then-divide.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_mul_div_u(uint32_t a, uint32_t b, uint32_t c);
 *     - Return floor((a * b) / c) via a 64-bit intermediate product so
 *       a*b never wraps in uint32_t before the divide. c==0 → 0.
 *   uint32_t __gj_u32_mul_div_u  (alias)
 *   __libcgj_batch3051_marker = "libcgj-batch3051"
 *
 * CREATE-ONLY exclusive continuum wave (3051-3060). Unique
 * gj_u32_mul_div_u surface only; no parent wires, no multi-def.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3051_marker[] = "libcgj-batch3051";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * floor((a * b) / c) with a full-width product in uint64_t.
 * Division by zero soft-returns 0 (no errno; freestanding-safe).
 */
static uint32_t
b3051_mul_div(uint32_t u32A, uint32_t u32B, uint32_t u32C)
{
	uint64_t u64Prod;

	if (u32C == 0u) {
		return 0u;
	}
	u64Prod = (uint64_t)u32A * (uint64_t)u32B;
	return (uint32_t)(u64Prod / (uint64_t)u32C);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_mul_div_u - portable floor((a * b) / c) for uint32_t.
 *
 * a, b: factors
 * c:    divisor (c==0 yields 0)
 *
 * The product is formed in uint64_t so the multiply cannot wrap before
 * the divide. Result is truncated toward zero (unsigned). No __int128.
 */
uint32_t
gj_u32_mul_div_u(uint32_t u32A, uint32_t u32B, uint32_t u32C)
{
	(void)NULL;
	return b3051_mul_div(u32A, u32B, u32C);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_mul_div_u(uint32_t u32A, uint32_t u32B, uint32_t u32C)
    __attribute__((alias("gj_u32_mul_div_u")));
