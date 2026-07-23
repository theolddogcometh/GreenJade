/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2371: saturating uint8_t add.
 *
 * Surface (unique symbols):
 *   uint8_t gj_u8_sat_add(uint8_t a, uint8_t b);
 *     - a + b with saturation at UINT8_MAX on overflow (no wrap).
 *   uint8_t __gj_u8_sat_add  (alias)
 *   __libcgj_batch2371_marker = "libcgj-batch2371"
 *
 * Post-2370 unsigned saturating arith exclusive wave (2371-2380).
 * Distinct from gj_u32_sat_add (batch923) and gj_u64_sat_add (batch921)
 * - 8-bit surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2371_marker[] = "libcgj-batch2371";

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating a + b in the uint8_t domain. */
static uint8_t
b2371_add_sat(uint8_t u8A, uint8_t u8B)
{
	if (u8B > (uint8_t)(UINT8_MAX - u8A)) {
		return UINT8_MAX;
	}
	return (uint8_t)(u8A + u8B);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u8_sat_add - unsigned 8-bit add; clamp to UINT8_MAX on overflow.
 *
 * a, b: addends
 *
 * Detect overflow without wrapping: b > UINT8_MAX - a.
 */
uint8_t
gj_u8_sat_add(uint8_t u8A, uint8_t u8B)
{
	(void)NULL;
	return b2371_add_sat(u8A, u8B);
}

/* ---- underscored alias ------------------------------------------------- */

uint8_t __gj_u8_sat_add(uint8_t u8A, uint8_t u8B)
    __attribute__((alias("gj_u8_sat_add")));
