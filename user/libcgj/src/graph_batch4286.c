/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4286: ASCII uppercase of one unsigned code
 * unit (_u).
 *
 * Surface (unique symbols):
 *   unsigned gj_ascii_toupper_u2(unsigned c);
 *     - Map a..z (0x61..0x7A) to A..Z; leave all other values
 *       unchanged. Returns the unsigned char value as unsigned.
 *   unsigned __gj_ascii_toupper_u2  (alias)
 *   __libcgj_batch4286_marker = "libcgj-batch4286"
 *
 * Exclusive continuum CREATE-ONLY (4281-4290): ASCII classify unique
 * _u returns (unsigned code unit). Distinct from gj_ascii_toupper
 * (in-place string, batch355) and ctype toupper. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4286_marker[] = "libcgj-batch4286";

/* ---- freestanding helpers ---------------------------------------------- */

/* Map one code unit: a..z -> A..Z; else identity. */
static unsigned
b4286_to_upper(unsigned uC)
{
	if (uC >= (unsigned)'a' && uC <= (unsigned)'z') {
		return uC - ((unsigned)'a' - (unsigned)'A');
	}
	return uC;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ascii_toupper_u2 - fold one ASCII lowercase letter to uppercase.
 *
 * c: input code unit
 *
 * Only 0x61..0x7A change; all other values are returned unchanged.
 * Result is the unsigned character value.
 */
unsigned
gj_ascii_toupper_u2(unsigned uC)
{
	(void)NULL;
	return b4286_to_upper(uC);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_ascii_toupper_u2(unsigned uC)
    __attribute__((alias("gj_ascii_toupper_u2")));
