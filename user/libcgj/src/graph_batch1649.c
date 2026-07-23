/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1649: ASCII hex nibble value (0..15 or -1).
 *
 * Surface (unique symbols):
 *   int gj_hex_nibble_value(int c);
 *     — If c is a hex digit ('0'-'9', 'a'-'f', 'A'-'F'), return its
 *       nibble value 0..15. Otherwise return -1.
 *   int __gj_hex_nibble_value  (alias)
 *   __libcgj_batch1649_marker = "libcgj-batch1649"
 *
 * Distinct from gj_hex_nibble (graph_batch531) — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

const char __libcgj_batch1649_marker[] = "libcgj-batch1649";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1649_nibble(int nC)
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
 * gj_hex_nibble_value — map one ASCII hex digit to a 4-bit value.
 *
 * c: candidate character (typically an unsigned char promoted to int).
 * Returns 0..15 for valid hex digits, or -1 if c is not in
 * 0-9 / a-f / A-F. Case-insensitive for letters. Pure; no side effects.
 */
int
gj_hex_nibble_value(int nC)
{
	return b1649_nibble(nC);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_hex_nibble_value(int nC)
    __attribute__((alias("gj_hex_nibble_value")));
