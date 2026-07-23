/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4152: unsigned Q16.16 divide (_q16x).
 *
 * Surface (unique symbols):
 *   uint32_t gj_q16x_div_u(uint32_t a, uint32_t b);
 *     - Q16.16 quotient: b==0 → 0; else ((uint64_t)a << 16) / b.
 *   uint32_t __gj_q16x_div_u  (alias)
 *   __libcgj_batch4152_marker = "libcgj-batch4152"
 *
 * Exclusive continuum CREATE-ONLY (4151-4160). Distinct from
 * gj_q16_div (batch172) — unique gj_q16x_div_u surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4152_marker[] = "libcgj-batch4152";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b4152_div — Q16.16 divide; zero divisor yields 0 (no trap).
 */
static uint32_t
b4152_div(uint32_t u32A, uint32_t u32B)
{
	uint64_t u64Num;

	if (u32B == 0u) {
		return 0u;
	}
	u64Num = ((uint64_t)u32A) << 16;
	return (uint32_t)(u64Num / (uint64_t)u32B);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_q16x_div_u - unsigned Q16.16 divide.
 *
 * a: Q16.16 dividend; b: Q16.16 divisor
 *
 * Returns 0 when b is 0; otherwise ((uint64_t)a << 16) / b as uint32_t.
 * No parent wires.
 */
uint32_t
gj_q16x_div_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b4152_div(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_q16x_div_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_q16x_div_u")));
