/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2372: saturating uint8_t subtract.
 *
 * Surface (unique symbols):
 *   uint8_t gj_u8_sat_sub(uint8_t a, uint8_t b);
 *     - a - b with saturation at 0 on underflow (no wrap).
 *   uint8_t __gj_u8_sat_sub  (alias)
 *   __libcgj_batch2372_marker = "libcgj-batch2372"
 *
 * Post-2370 unsigned saturating arith exclusive wave (2371-2380).
 * Distinct from gj_u32_sat_sub (batch924) and gj_u64_sat_sub - 8-bit
 * surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2372_marker[] = "libcgj-batch2372";

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating a - b in the uint8_t domain. */
static uint8_t
b2372_sub_sat(uint8_t u8A, uint8_t u8B)
{
	if (u8A < u8B) {
		return 0u;
	}
	return (uint8_t)(u8A - u8B);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u8_sat_sub - unsigned 8-bit subtract; clamp to 0 on underflow.
 *
 * a: minuend
 * b: subtrahend
 *
 * Underflow when a < b.
 */
uint8_t
gj_u8_sat_sub(uint8_t u8A, uint8_t u8B)
{
	(void)NULL;
	return b2372_sub_sat(u8A, u8B);
}

/* ---- underscored alias ------------------------------------------------- */

uint8_t __gj_u8_sat_sub(uint8_t u8A, uint8_t u8B)
    __attribute__((alias("gj_u8_sat_sub")));
