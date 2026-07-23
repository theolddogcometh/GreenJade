/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3257: uint32 ratio as unsigned Q16.16 (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_ratio_q16_u(uint32_t a, uint32_t b);
 *     - Return floor((a * 65536) / b) as an unsigned Q16.16 fixed-point
 *       ratio of a/b (1.0 == 0x00010000). b==0 → 0. Product formed in
 *       uint64_t so a*65536 never wraps in uint32_t before the divide.
 *   uint32_t __gj_u32_ratio_q16_u  (alias)
 *   __libcgj_batch3257_marker = "libcgj-batch3257"
 *
 * CREATE-ONLY exclusive continuum wave (3251-3260). Unique
 * gj_u32_ratio_q16_u surface only; no multi-def. Distinct from
 * gj_ratio_u64 (batch998) and gj_u32_mul_div_u (batch3051).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3257_marker[] = "libcgj-batch3257";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * floor((a << 16) / b) via a 64-bit intermediate product.
 * Division by zero soft-returns 0 (no errno; freestanding-safe).
 * Result truncated to low 32 bits of the mathematical quotient.
 */
static uint32_t
b3257_ratio_q16(uint32_t u32A, uint32_t u32B)
{
	uint64_t u64Prod;

	if (u32B == 0u) {
		return 0u;
	}
	u64Prod = (uint64_t)u32A << 16;
	return (uint32_t)(u64Prod / (uint64_t)u32B);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_ratio_q16_u - unsigned Q16.16 ratio floor((a * 65536) / b).
 *
 * a: numerator
 * b: denominator (b==0 yields 0)
 *
 * Self-contained; no parent wires to mul_div / q16 helpers.
 */
uint32_t
gj_u32_ratio_q16_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b3257_ratio_q16(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_ratio_q16_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_u32_ratio_q16_u")));
