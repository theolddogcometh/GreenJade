/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2903: saturating uint16_t add (_u).
 *
 * Surface (unique symbols):
 *   uint16_t gj_u16_sat_add_u(uint16_t a, uint16_t b);
 *     - a + b with saturation at UINT16_MAX on overflow (no wrap).
 *   uint16_t __gj_u16_sat_add_u  (alias)
 *   __libcgj_batch2903_marker = "libcgj-batch2903"
 *
 * Continuum sat-arith exclusive wave (2901-2910). Distinct from
 * gj_u16_sat_add (batch2373) — unique _u name; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2903_marker[] = "libcgj-batch2903";

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating a + b in the uint16_t domain. */
static uint16_t
b2903_add_sat(uint16_t u16A, uint16_t u16B)
{
	if (u16B > (uint16_t)(UINT16_MAX - u16A)) {
		return UINT16_MAX;
	}
	return (uint16_t)(u16A + u16B);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u16_sat_add_u - unsigned 16-bit add; clamp to UINT16_MAX on overflow.
 *
 * a, b: addends
 *
 * Detect overflow without wrapping: b > UINT16_MAX - a.
 */
uint16_t
gj_u16_sat_add_u(uint16_t u16A, uint16_t u16B)
{
	(void)NULL;
	return b2903_add_sat(u16A, u16B);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_u16_sat_add_u(uint16_t u16A, uint16_t u16B)
    __attribute__((alias("gj_u16_sat_add_u")));
