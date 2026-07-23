/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4289: ASCII hex nibble value (_u, unsigned
 * code unit; 0xff invalid).
 *
 * Surface (unique symbols):
 *   unsigned gj_hex_nibble_val_u(unsigned c);
 *     - If c is a hex digit ('0'-'9', 'a'-'f', 'A'-'F'), return its
 *       nibble value 0..15. Otherwise return 0xff (invalid).
 *   unsigned __gj_hex_nibble_val_u  (alias)
 *   __libcgj_batch4289_marker = "libcgj-batch4289"
 *
 * Exclusive continuum CREATE-ONLY (4281-4290): ASCII classify unique
 * _u returns (unsigned code unit). Distinct from gj_hex_nibble
 * (batch531, int/-1) and gj_hex_nibble_value (batch1649, int/-1) —
 * unique gj_hex_nibble_val_u surface; invalid is 0xff not -1. No
 * parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4289_marker[] = "libcgj-batch4289";

/* Invalid nibble sentinel (not a 4-bit value). */
#define B4289_INV  0xffu

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Map one ASCII hex digit to 0..15; else B4289_INV.
 */
static unsigned
b4289_nibble(unsigned uC)
{
	if (uC >= (unsigned)'0' && uC <= (unsigned)'9') {
		return uC - (unsigned)'0';
	}
	if (uC >= (unsigned)'a' && uC <= (unsigned)'f') {
		return uC - (unsigned)'a' + 10u;
	}
	if (uC >= (unsigned)'A' && uC <= (unsigned)'F') {
		return uC - (unsigned)'A' + 10u;
	}
	return B4289_INV;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hex_nibble_val_u - map one ASCII hex digit to a 4-bit value.
 *
 * c: candidate code unit (typically a promoted octet)
 *
 * Returns 0..15 for valid hex digits, or 0xff if c is not in
 * 0-9 / a-f / A-F. Case-insensitive for letters. Pure; no side effects.
 */
unsigned
gj_hex_nibble_val_u(unsigned uC)
{
	(void)NULL;
	return b4289_nibble(uC);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_hex_nibble_val_u(unsigned uC)
    __attribute__((alias("gj_hex_nibble_val_u")));
