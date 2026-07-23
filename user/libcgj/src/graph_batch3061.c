/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3061: unsigned Q16.16 divide (exclusive _u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_q16_div_u(uint32_t a, uint32_t b);
 *     - Unsigned Q16.16: ((uint64_t)a << 16) / b. Div-by-zero → 0.
 *       Quotient truncated to uint32_t (wrap on overflow).
 *   uint32_t __gj_q16_div_u  (alias)
 *   __libcgj_batch3061_marker = "libcgj-batch3061"
 *
 * Distinct from gj_q16_div (batch172) and gj_q16_div_sat (batch1472) —
 * exclusive unsigned _u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3061_marker[] = "libcgj-batch3061";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_q16_div_u — divide two unsigned Q16.16 values.
 *
 * Numerator is shifted left by 16 before division. Division by zero
 * returns 0 (no errno; freestanding-safe). No parent wires.
 */
uint32_t
gj_q16_div_u(uint32_t u32A, uint32_t u32B)
{
	uint64_t u64Num;

	(void)NULL;
	if (u32B == 0u) {
		return 0u;
	}
	u64Num = (uint64_t)u32A << 16;
	return (uint32_t)(u64Num / (uint64_t)u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_q16_div_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_q16_div_u")));
