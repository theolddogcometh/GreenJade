/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch847: uint32 factorial; n>12 → 0.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fact_u32(unsigned n);
 *     — n! for n in [0,12]. 0! = 1. n > 12 → 0 (13! overflows u32).
 *   uint32_t __gj_fact_u32  (alias)
 *   __libcgj_batch847_marker = "libcgj-batch847"
 *
 * Distinct from gj_fact_u64 (batch384) — no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch847_marker[] = "libcgj-batch847";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fact_u32 — n! for n in [0, 12]; 0 on overflow (n > 12).
 *
 * 12! = 479001600 fits in uint32_t; 13! = 6227020800 does not.
 */
uint32_t
gj_fact_u32(unsigned uN)
{
	uint32_t u32Prod;
	unsigned uI;

	if (uN > 12u) {
		return 0u;
	}

	u32Prod = 1u;
	for (uI = 2u; uI <= uN; uI++) {
		u32Prod *= (uint32_t)uI;
	}
	return u32Prod;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fact_u32(unsigned uN)
    __attribute__((alias("gj_fact_u32")));
