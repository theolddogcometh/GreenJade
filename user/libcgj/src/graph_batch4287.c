/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4287: ASCII lowercase of one unsigned code
 * unit (_u).
 *
 * Surface (unique symbols):
 *   unsigned gj_ascii_tolower_u2(unsigned c);
 *     - Map A..Z (0x41..0x5A) to a..z; leave all other values
 *       unchanged. Returns the unsigned char value as unsigned.
 *   unsigned __gj_ascii_tolower_u2  (alias)
 *   __libcgj_batch4287_marker = "libcgj-batch4287"
 *
 * Exclusive continuum CREATE-ONLY (4281-4290): ASCII classify unique
 * _u returns (unsigned code unit). Distinct from gj_ascii_tolower
 * (in-place string, batch355) and ctype tolower. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4287_marker[] = "libcgj-batch4287";

/* ---- freestanding helpers ---------------------------------------------- */

/* Map one code unit: A..Z -> a..z; else identity. */
static unsigned
b4287_to_lower(unsigned uC)
{
	if (uC >= (unsigned)'A' && uC <= (unsigned)'Z') {
		return uC + ((unsigned)'a' - (unsigned)'A');
	}
	return uC;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ascii_tolower_u2 - fold one ASCII uppercase letter to lowercase.
 *
 * c: input code unit
 *
 * Only 0x41..0x5A change; all other values are returned unchanged.
 * Result is the unsigned character value.
 */
unsigned
gj_ascii_tolower_u2(unsigned uC)
{
	(void)NULL;
	return b4287_to_lower(uC);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_ascii_tolower_u2(unsigned uC)
    __attribute__((alias("gj_ascii_tolower_u2")));
