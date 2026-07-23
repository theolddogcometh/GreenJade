/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8454: low nibble of octet → hex digit stub.
 *
 * Surface (unique symbols):
 *   int gj_hex_byte_lo_u_8454(unsigned b);
 *     - Return the lowercase hex digit for the low nibble of b
 *       (b & 0xF → '0'-'9' / 'a'-'f' as int). Only the low 8 bits of
 *       b participate for the low nibble mask.
 *   int __gj_hex_byte_lo_u_8454  (alias)
 *   __libcgj_batch8454_marker = "libcgj-batch8454"
 *
 * Exclusive continuum CREATE-ONLY (8451-8460: hex encode/decode stubs).
 * Unique gj_hex_byte_lo_u_8454 surface only; no multi-def. Companion to
 * gj_hex_byte_hi_u_8453. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8454_marker[] = "libcgj-batch8454";

/* ---- freestanding helpers ---------------------------------------------- */

/* Low nibble of an octet as lowercase hex digit. */
static int
b8454_byte_lo(unsigned uB)
{
	unsigned uNib;

	uNib = uB & 0x0fu;
	if (uNib < 10u) {
		return (int)('0' + uNib);
	}
	return (int)('a' + (uNib - 10u));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hex_byte_lo_u_8454 - low-nibble hex digit of one octet.
 *
 * b: source value; low nibble is bits 3..0.
 *
 * Returns '0'..'9' / 'a'..'f' as int. Pure; no I/O. No parent wires.
 */
int
gj_hex_byte_lo_u_8454(unsigned uB)
{
	(void)NULL;
	return b8454_byte_lo(uB);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_hex_byte_lo_u_8454(unsigned uB)
    __attribute__((alias("gj_hex_byte_lo_u_8454")));
