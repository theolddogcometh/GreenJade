/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2796: ASCII hex digit → nibble (exclusive _u).
 *
 * Surface (unique symbols):
 *   int gj_hex_nibble_u(int c);
 *     - If c is a hex digit ('0'-'9', 'a'-'f', 'A'-'F'), return its
 *       nibble value 0..15. Otherwise return -1.
 *   int __gj_hex_nibble_u  (alias)
 *   __libcgj_batch2796_marker = "libcgj-batch2796"
 *
 * Milestone 2800 exclusive byte/hex helpers (2791-2800). Unique
 * gj_hex_nibble_u surface only; no multi-def. Distinct from
 * gj_hex_nibble (batch531), gj_hex_nibble_value (batch1649),
 * gj_hex_nibble_from_char_u (batch2588), and gj_from_hex_nibble_u
 * (batch2797 inverse).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2796_marker[] = "libcgj-batch2796";

/* ---- freestanding helpers ---------------------------------------------- */

/* Map one ASCII hex digit to 0..15, or -1 if not hex. */
static int
b2796_nibble(int nC)
{
	if (nC >= '0' && nC <= '9') {
		return nC - '0';
	}
	if (nC >= 'a' && nC <= 'f') {
		return nC - 'a' + 10;
	}
	if (nC >= 'A' && nC <= 'F') {
		return nC - 'A' + 10;
	}
	return -1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hex_nibble_u - map one ASCII hex digit to a 4-bit value.
 *
 * c: candidate character (typically an unsigned char promoted to int).
 *
 * Returns 0..15 for valid hex digits, or -1 if c is not in
 * 0-9 / a-f / A-F. Case-insensitive for letters. Pure; no side effects.
 */
int
gj_hex_nibble_u(int nC)
{
	(void)NULL;
	return b2796_nibble(nC);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_hex_nibble_u(int nC)
    __attribute__((alias("gj_hex_nibble_u")));
