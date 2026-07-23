/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8261: saturating uint32_t add.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sat_add_u32_8261(uint32_t a, uint32_t b);
 *     - a + b with saturation at UINT32_MAX on overflow (no wrap).
 *   uint32_t __gj_sat_add_u32_8261  (alias)
 *   __libcgj_batch8261_marker = "libcgj-batch8261"
 *
 * Exclusive continuum CREATE-ONLY (8261-8270: saturating arithmetic
 * stubs uint32 — sat_add, sat_sub, sat_mul, sat_inc, sat_dec, clamp,
 * min, max, absdiff, batch_id_8270). Unique gj_sat_add_u32_8261
 * surface only; no multi-def. Distinct from gj_sat_add_u64
 * (batch377). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8261_marker[] = "libcgj-batch8261";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Saturating unsigned 32-bit add. Detect overflow without wrapping:
 * b > UINT32_MAX - a.
 */
static uint32_t
b8261_sat_add(uint32_t u32A, uint32_t u32B)
{
	if (u32B > (UINT32_MAX - u32A)) {
		return UINT32_MAX;
	}
	return u32A + u32B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sat_add_u32_8261 - unsigned add; clamp to UINT32_MAX if a + b
 * overflows.
 *
 * No parent wires. Does not call libc.
 */
uint32_t
gj_sat_add_u32_8261(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b8261_sat_add(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sat_add_u32_8261(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_sat_add_u32_8261")));
