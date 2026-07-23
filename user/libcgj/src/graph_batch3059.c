/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3059: unsigned Q16.16 multiply (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_q16_mul_u(uint32_t a, uint32_t b);
 *     - (a * b) >> 16 via a uint64 intermediate. Operands and result
 *       are unsigned Q16.16 (1.0 == 0x00010000). High bits above bit 31
 *       of the shifted product are discarded (modular wrap to uint32).
 *   uint32_t __gj_q16_mul_u  (alias)
 *   __libcgj_batch3059_marker = "libcgj-batch3059"
 *
 * CREATE-ONLY exclusive continuum wave (3051-3060). Unique gj_q16_mul_u
 * surface only; no parent wires, no multi-def. Distinct from signed
 * gj_q16_mul (batch172) and gj_q16_mul_sat (batch1471). No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3059_marker[] = "libcgj-batch3059";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Unsigned Q16.16 multiply: product in uint64_t, restore scale >> 16.
 */
static uint32_t
b3059_q16_mul(uint32_t u32A, uint32_t u32B)
{
	uint64_t u64Prod;

	u64Prod = (uint64_t)u32A * (uint64_t)u32B;
	return (uint32_t)(u64Prod >> 16);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_q16_mul_u - multiply two unsigned Q16.16 values.
 *
 * a, b: unsigned Q16.16 factors (1.0 == 0x00010000)
 *
 * Product is computed in uint64_t then shifted right by 16 to restore
 * Q16.16 scale. Result is the low 32 bits of the shifted product.
 * Self-contained; no parent wires to signed q16 helpers.
 */
uint32_t
gj_q16_mul_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b3059_q16_mul(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_q16_mul_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_q16_mul_u")));
