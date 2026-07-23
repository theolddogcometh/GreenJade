/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2949: uint32 factorial; n>12 → 0 (_u surface).
 *
 * Surface (unique symbols):
 *   uint32_t gj_fact_u32_u(unsigned n);
 *     — n! for n in [0,12]. 0! = 1. n > 12 → 0 (13! overflows u32).
 *   uint32_t __gj_fact_u32_u  (alias)
 *   __libcgj_batch2949_marker = "libcgj-batch2949"
 *
 * CREATE-ONLY exclusive continuum wave (2941–2950). Distinct from
 * gj_fact_u32 (batch847) and gj_fact_u64 (batch384) — unique
 * gj_fact_u32_u name; no parent wires, no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2949_marker[] = "libcgj-batch2949";

/* ---- freestanding helpers ---------------------------------------------- */

/* n! for n in [0,12]; 0 on overflow (n > 12). */
static uint32_t
b2949_fact(unsigned uN)
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

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fact_u32_u — n! for n in [0, 12]; 0 on overflow (n > 12).
 *
 * 12! = 479001600 fits in uint32_t; 13! = 6227020800 does not.
 */
uint32_t
gj_fact_u32_u(unsigned uN)
{
	(void)NULL;
	return b2949_fact(uN);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fact_u32_u(unsigned uN)
    __attribute__((alias("gj_fact_u32_u")));
