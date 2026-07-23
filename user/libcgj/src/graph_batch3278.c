/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3278: ASCII hex digit → nibble value.
 *
 * Surface (unique symbols):
 *   int gj_ascii_fromhex_u(unsigned char c);
 *     - If c is a hex digit ('0'-'9', 'a'-'f', 'A'-'F'), return its
 *       nibble value 0..15. Otherwise return -1.
 *   int __gj_ascii_fromhex_u  (alias)
 *   __libcgj_batch3278_marker = "libcgj-batch3278"
 *
 * ASCII exclusive wave (3271-3280). Distinct from
 * gj_hex_nibble_from_char_u (batch2588, int arg) and
 * gj_ascii_isxdigit_u (batch2817 predicate) — unique fromhex_u surface;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3278_marker[] = "libcgj-batch3278";

/* ---- freestanding helpers ---------------------------------------------- */

/* Map one ASCII hex digit to 0..15, or -1 if not hex. */
static int
b3278_fromhex(unsigned char u8C)
{
	if (u8C >= (unsigned char)'0' && u8C <= (unsigned char)'9') {
		return (int)(u8C - (unsigned char)'0');
	}
	if (u8C >= (unsigned char)'a' && u8C <= (unsigned char)'f') {
		return (int)(u8C - (unsigned char)'a') + 10;
	}
	if (u8C >= (unsigned char)'A' && u8C <= (unsigned char)'F') {
		return (int)(u8C - (unsigned char)'A') + 10;
	}
	return -1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ascii_fromhex_u - map one ASCII hex digit to a 4-bit value.
 *
 * c: candidate octet (0..255).
 * Returns 0..15 for valid hex digits, or -1 if c is not in
 * 0-9 / a-f / A-F. Case-insensitive for letters. Pure; no side effects.
 */
int
gj_ascii_fromhex_u(unsigned char u8C)
{
	(void)NULL;
	return b3278_fromhex(u8C);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ascii_fromhex_u(unsigned char u8C)
    __attribute__((alias("gj_ascii_fromhex_u")));
