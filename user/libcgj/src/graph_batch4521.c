/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4521: freestanding 4-bit table lookup (_u).
 *
 * Surface (unique symbols):
 *   uint8_t gj_u8_lut4_u(const uint8_t lut[16], uint8_t x);
 *     - Index lut by the low nibble of x (x & 15). High bits of x are
 *       ignored. Returns 0 when lut is NULL.
 *   uint8_t __gj_u8_lut4_u  (alias)
 *   __libcgj_batch4521_marker = "libcgj-batch4521"
 *
 * Exclusive continuum CREATE-ONLY (4521-4530: table-lookup unique wave).
 * Unique gj_u8_lut4_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4521_marker[] = "libcgj-batch4521";

/* ---- freestanding helpers ---------------------------------------------- */

static uint8_t
b4521_lut4(const uint8_t *pLut, uint8_t u8X)
{
	if (pLut == NULL) {
		return 0u;
	}
	return pLut[(unsigned)(u8X & 0x0fu)];
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u8_lut4_u - 4-bit (nibble) table lookup.
 *
 * lut: 16-entry substitution table (may be NULL -> 0)
 * x:   index source; only bits 0..3 select the entry
 *
 * Returns lut[x & 15], or 0 if lut is NULL. Self-contained; no parent wires.
 */
uint8_t
gj_u8_lut4_u(const uint8_t pLut[16], uint8_t u8X)
{
	(void)NULL;
	return b4521_lut4(pLut, u8X);
}

/* ---- underscored alias ------------------------------------------------- */

uint8_t __gj_u8_lut4_u(const uint8_t pLut[16], uint8_t u8X)
    __attribute__((alias("gj_u8_lut4_u")));
