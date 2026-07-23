/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2708: saturating uint32_t subtract (sub_sat_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_sub_sat_u(uint32_t a, uint32_t b);
 *     - a - b with saturation at 0 on underflow (no wrap).
 *   uint32_t __gj_u32_sub_sat_u  (alias)
 *   __libcgj_batch2708_marker = "libcgj-batch2708"
 *
 * Post-2700 product/Steam integer helpers exclusive wave (2701-2710).
 * Distinct from gj_u32_sat_sub (batch924) and gj_u32_sat_sub_u
 * (batch2376) — unique sub_sat_u name order; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2708_marker[] = "libcgj-batch2708";

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating a - b in the uint32_t domain. */
static uint32_t
b2708_sub_sat(uint32_t u32A, uint32_t u32B)
{
	if (u32A < u32B) {
		return 0u;
	}
	return u32A - u32B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_sub_sat_u - unsigned 32-bit subtract; clamp to 0 on underflow.
 *
 * a: minuend
 * b: subtrahend
 *
 * Underflow when a < b.
 */
uint32_t
gj_u32_sub_sat_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b2708_sub_sat(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_sub_sat_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_u32_sub_sat_u")));
