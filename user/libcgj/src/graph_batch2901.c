/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2901: saturating uint8_t add (_u).
 *
 * Surface (unique symbols):
 *   uint8_t gj_u8_sat_add_u(uint8_t a, uint8_t b);
 *     - a + b with saturation at UINT8_MAX on overflow (no wrap).
 *   uint8_t __gj_u8_sat_add_u  (alias)
 *   __libcgj_batch2901_marker = "libcgj-batch2901"
 *
 * Continuum sat-arith exclusive wave (2901-2910). Distinct from
 * gj_u8_sat_add (batch2371) — unique _u name; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2901_marker[] = "libcgj-batch2901";

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating a + b in the uint8_t domain. */
static uint8_t
b2901_add_sat(uint8_t u8A, uint8_t u8B)
{
	if (u8B > (uint8_t)(UINT8_MAX - u8A)) {
		return UINT8_MAX;
	}
	return (uint8_t)(u8A + u8B);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u8_sat_add_u - unsigned 8-bit add; clamp to UINT8_MAX on overflow.
 *
 * a, b: addends
 *
 * Detect overflow without wrapping: b > UINT8_MAX - a.
 */
uint8_t
gj_u8_sat_add_u(uint8_t u8A, uint8_t u8B)
{
	(void)NULL;
	return b2901_add_sat(u8A, u8B);
}

/* ---- underscored alias ------------------------------------------------- */

uint8_t __gj_u8_sat_add_u(uint8_t u8A, uint8_t u8B)
    __attribute__((alias("gj_u8_sat_add_u")));
