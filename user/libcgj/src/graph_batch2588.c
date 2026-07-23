/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2588: hex nibble from char (exclusive _u).
 *
 * Surface (unique symbols):
 *   int gj_hex_nibble_from_char_u(int c);
 *     - If c is a hex digit ('0'-'9', 'a'-'f', 'A'-'F'), return its
 *       nibble value 0..15. Otherwise return -1.
 *   int __gj_hex_nibble_from_char_u  (alias)
 *   __libcgj_batch2588_marker = "libcgj-batch2588"
 *
 * Distinct from gj_hex_nibble (batch531) and gj_hex_nibble_value
 * (batch1649) — exclusive _u surface.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2588_marker[] = "libcgj-batch2588";

/* ---- freestanding helpers ---------------------------------------------- */

/* Map one ASCII hex digit to 0..15, or -1 if not hex. */
static int
b2588_nibble(int nC)
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
 * gj_hex_nibble_from_char_u — map one ASCII hex digit to a 4-bit value.
 *
 * c: candidate character (typically an unsigned char promoted to int).
 * Returns 0..15 for valid hex digits, or -1 if c is not in
 * 0-9 / a-f / A-F. Case-insensitive for letters. Pure; no side effects.
 */
int
gj_hex_nibble_from_char_u(int nC)
{
	(void)NULL;
	return b2588_nibble(nC);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_hex_nibble_from_char_u(int nC)
    __attribute__((alias("gj_hex_nibble_from_char_u")));
