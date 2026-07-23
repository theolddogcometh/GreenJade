/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3053: uint32 percentage-of value (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_pct_u(uint32_t x, uint32_t pct);
 *     - Return floor((x * pct) / 100). Product formed in uint64_t so
 *       x*pct does not wrap in uint32_t before the divide.
 *   uint32_t __gj_u32_pct_u  (alias)
 *   __libcgj_batch3053_marker = "libcgj-batch3053"
 *
 * CREATE-ONLY exclusive continuum wave (3051-3060). Unique gj_u32_pct_u
 * surface only; no parent wires, no multi-def. Distinct from
 * gj_pct_of_u64 (batch996) and gj_percent_of_u64 (batch1746).
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3053_marker[] = "libcgj-batch3053";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3053_pct(uint32_t u32X, uint32_t u32Pct)
{
	uint64_t u64Prod;

	if (u32X == 0u || u32Pct == 0u) {
		return 0u;
	}
	u64Prod = (uint64_t)u32X * (uint64_t)u32Pct;
	return (uint32_t)(u64Prod / 100ull);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_pct_u - compute floor((x * pct) / 100) for uint32_t budgets.
 *
 * x:   base value
 * pct: percentage (100 = 100%)
 *
 * Intermediate product is uint64_t; result truncated toward zero.
 * Self-contained: no parent wires to mul_div helpers.
 */
uint32_t
gj_u32_pct_u(uint32_t u32X, uint32_t u32Pct)
{
	(void)NULL;
	return b3053_pct(u32X, u32Pct);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_pct_u(uint32_t u32X, uint32_t u32Pct)
    __attribute__((alias("gj_u32_pct_u")));
