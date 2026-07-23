/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4277: saturating uint32_t add with flag (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_add_sat_flag_u(uint32_t a, uint32_t b, uint32_t *flag);
 *     - a + b with saturation at UINT32_MAX on overflow (no wrap).
 *       When flag is non-NULL, *flag = 1 if saturated, else *flag = 0.
 *       flag may be NULL (status ignored).
 *   uint32_t __gj_u32_add_sat_flag_u  (alias)
 *   __libcgj_batch4277_marker = "libcgj-batch4277"
 *
 * Exclusive continuum CREATE-ONLY (4271-4280): checked arith unique.
 * Distinct from gj_u64_add_sat (batch2031) / gj_sat_add_u64 (batch377)
 * / gj_u32_add_overflow_u (batch4271) — unique surface; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4277_marker[] = "libcgj-batch4277";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Saturating a + b; optional *flag = 1 when clamped to UINT32_MAX.
 */
static uint32_t
b4277_add_sat(uint32_t u32A, uint32_t u32B, uint32_t *pFlag)
{
	uint32_t u32Sum;
	int nSat;

	if (u32B > (UINT32_MAX - u32A)) {
		u32Sum = UINT32_MAX;
		nSat = 1;
	} else {
		u32Sum = u32A + u32B;
		nSat = 0;
	}
	if (pFlag != NULL) {
		*pFlag = (uint32_t)nSat;
	}
	return u32Sum;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_add_sat_flag_u - saturating unsigned add with optional flag.
 *
 * a, b:  addends
 * flag:  optional; *flag = 1 if result was saturated, else 0
 *
 * Returns a + b clamped to UINT32_MAX. No parent wires.
 */
uint32_t
gj_u32_add_sat_flag_u(uint32_t a, uint32_t b, uint32_t *flag)
{
	(void)NULL;
	return b4277_add_sat(a, b, flag);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_add_sat_flag_u(uint32_t a, uint32_t b, uint32_t *flag)
    __attribute__((alias("gj_u32_add_sat_flag_u")));
