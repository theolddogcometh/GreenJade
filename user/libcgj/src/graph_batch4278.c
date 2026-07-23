/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4278: saturating uint32_t sub with flag (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_sub_sat_flag_u(uint32_t a, uint32_t b, uint32_t *flag);
 *     - a - b with saturation at 0 on underflow (no wrap).
 *       When flag is non-NULL, *flag = 1 if saturated, else *flag = 0.
 *       flag may be NULL (status ignored).
 *   uint32_t __gj_u32_sub_sat_flag_u  (alias)
 *   __libcgj_batch4278_marker = "libcgj-batch4278"
 *
 * Exclusive continuum CREATE-ONLY (4271-4280): checked arith unique.
 * Distinct from gj_u64_sub_sat (batch2032) / gj_u32_sub_overflow_u
 * (batch4272) / gj_u32_add_sat_flag_u (batch4277) — unique surface;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4278_marker[] = "libcgj-batch4278";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Saturating a - b; optional *flag = 1 when clamped to 0.
 */
static uint32_t
b4278_sub_sat(uint32_t u32A, uint32_t u32B, uint32_t *pFlag)
{
	uint32_t u32Diff;
	int nSat;

	if (u32A < u32B) {
		u32Diff = 0u;
		nSat = 1;
	} else {
		u32Diff = u32A - u32B;
		nSat = 0;
	}
	if (pFlag != NULL) {
		*pFlag = (uint32_t)nSat;
	}
	return u32Diff;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_sub_sat_flag_u - saturating unsigned sub with optional flag.
 *
 * a, b:  minuend / subtrahend
 * flag:  optional; *flag = 1 if result was saturated, else 0
 *
 * Returns a - b clamped to 0. No parent wires.
 */
uint32_t
gj_u32_sub_sat_flag_u(uint32_t a, uint32_t b, uint32_t *flag)
{
	(void)NULL;
	return b4278_sub_sat(a, b, flag);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_sub_sat_flag_u(uint32_t a, uint32_t b, uint32_t *flag)
    __attribute__((alias("gj_u32_sub_sat_flag_u")));
