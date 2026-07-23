/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8263: saturating uint32_t multiply.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sat_mul_u32_8263(uint32_t a, uint32_t b);
 *     - a * b with saturation at UINT32_MAX on overflow (no wrap).
 *   uint32_t __gj_sat_mul_u32_8263  (alias)
 *   __libcgj_batch8263_marker = "libcgj-batch8263"
 *
 * Exclusive continuum CREATE-ONLY (8261-8270: saturating arithmetic
 * stubs uint32 — sat_add, sat_sub, sat_mul, sat_inc, sat_dec, clamp,
 * min, max, absdiff, batch_id_8270). Unique gj_sat_mul_u32_8263
 * surface only; no multi-def. Distinct from gj_sat_mul_u64
 * (batch567). Uses uint64_t widen product only — no __int128.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8263_marker[] = "libcgj-batch8263";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Saturating unsigned 32-bit multiply via uint64_t widen product.
 * No __int128.
 */
static uint32_t
b8263_sat_mul(uint32_t u32A, uint32_t u32B)
{
	uint64_t u64Prod;

	u64Prod = (uint64_t)u32A * (uint64_t)u32B;
	if (u64Prod > (uint64_t)UINT32_MAX) {
		return UINT32_MAX;
	}
	return (uint32_t)u64Prod;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sat_mul_u32_8263 - unsigned multiply; clamp to UINT32_MAX if
 * a * b overflows.
 *
 * No parent wires. Does not call libc.
 */
uint32_t
gj_sat_mul_u32_8263(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b8263_sat_mul(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sat_mul_u32_8263(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_sat_mul_u32_8263")));
