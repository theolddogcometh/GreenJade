/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2374: saturating uint16_t subtract.
 *
 * Surface (unique symbols):
 *   uint16_t gj_u16_sat_sub(uint16_t a, uint16_t b);
 *     - a - b with saturation at 0 on underflow (no wrap).
 *   uint16_t __gj_u16_sat_sub  (alias)
 *   __libcgj_batch2374_marker = "libcgj-batch2374"
 *
 * Post-2370 unsigned saturating arith exclusive wave (2371-2380).
 * Distinct from gj_u8_sat_sub (batch2372) and gj_u32_sat_sub (batch924)
 * - 16-bit surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2374_marker[] = "libcgj-batch2374";

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating a - b in the uint16_t domain. */
static uint16_t
b2374_sub_sat(uint16_t u16A, uint16_t u16B)
{
	if (u16A < u16B) {
		return 0u;
	}
	return (uint16_t)(u16A - u16B);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u16_sat_sub - unsigned 16-bit subtract; clamp to 0 on underflow.
 *
 * a: minuend
 * b: subtrahend
 *
 * Underflow when a < b.
 */
uint16_t
gj_u16_sat_sub(uint16_t u16A, uint16_t u16B)
{
	(void)NULL;
	return b2374_sub_sat(u16A, u16B);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_u16_sat_sub(uint16_t u16A, uint16_t u16B)
    __attribute__((alias("gj_u16_sat_sub")));
