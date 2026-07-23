/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4705: saturating uint32_t decrement.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_sat_dec_u(uint32_t x);
 *     - x - 1 with saturation at 0 (no wrap).
 *   uint32_t __gj_u32_sat_dec_u  (alias)
 *   __libcgj_batch4705_marker = "libcgj-batch4705"
 *
 * Exclusive continuum CREATE-ONLY (4701-4710). Distinct from
 * gj_u32_sat_sub_u (batch2376) — unique gj_u32_sat_dec_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4705_marker[] = "libcgj-batch4705";

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating decrement: clamp at 0. */
static uint32_t
b4705_sat_dec(uint32_t u32X)
{
	if (u32X == 0u) {
		return 0u;
	}
	return u32X - 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_sat_dec_u - unsigned 32-bit saturating decrement.
 *
 * x: value to decrement
 *
 * Returns x - 1, or 0 when x is already 0.
 * Self-contained; no parent wires.
 */
uint32_t
gj_u32_sat_dec_u(uint32_t u32X)
{
	(void)NULL;
	return b4705_sat_dec(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_sat_dec_u(uint32_t u32X)
    __attribute__((alias("gj_u32_sat_dec_u")));
