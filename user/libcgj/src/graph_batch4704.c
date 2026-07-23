/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4704: saturating uint32_t increment.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_sat_inc_u(uint32_t x);
 *     - x + 1 with saturation at UINT32_MAX (no wrap).
 *   uint32_t __gj_u32_sat_inc_u  (alias)
 *   __libcgj_batch4704_marker = "libcgj-batch4704"
 *
 * Exclusive continuum CREATE-ONLY (4701-4710). Distinct from
 * gj_u32_sat_add_u (batch2375) — unique gj_u32_sat_inc_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4704_marker[] = "libcgj-batch4704";

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating increment: clamp at UINT32_MAX. */
static uint32_t
b4704_sat_inc(uint32_t u32X)
{
	if (u32X == UINT32_MAX) {
		return UINT32_MAX;
	}
	return u32X + 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_sat_inc_u - unsigned 32-bit saturating increment.
 *
 * x: value to increment
 *
 * Returns x + 1, or UINT32_MAX when x is already UINT32_MAX.
 * Self-contained; no parent wires.
 */
uint32_t
gj_u32_sat_inc_u(uint32_t u32X)
{
	(void)NULL;
	return b4704_sat_inc(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_sat_inc_u(uint32_t u32X)
    __attribute__((alias("gj_u32_sat_inc_u")));
