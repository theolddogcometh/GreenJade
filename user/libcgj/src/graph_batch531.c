/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch531: ASCII hex nibble decode.
 *
 * Surface (unique symbols):
 *   int gj_hex_nibble(int c);
 *     — If c is a hex digit ('0'-'9', 'a'-'f', 'A'-'F'), return its
 *       nibble value 0..15. Otherwise return -1.
 *   int __gj_hex_nibble  (alias)
 *   __libcgj_batch531_marker = "libcgj-batch531"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

const char __libcgj_batch531_marker[] = "libcgj-batch531";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hex_nibble — map one ASCII hex digit to a 4-bit value.
 *
 * c: candidate character (typically an unsigned char promoted to int).
 *
 * Returns 0..15 for valid hex digits, or -1 if c is not in
 * 0-9 / a-f / A-F. Case-insensitive for letters. Pure; no side effects.
 */
int
gj_hex_nibble(int c)
{
	if (c >= '0' && c <= '9') {
		return c - '0';
	}
	if (c >= 'a' && c <= 'f') {
		return c - 'a' + 10;
	}
	if (c >= 'A' && c <= 'F') {
		return c - 'A' + 10;
	}
	return -1;
}

int __gj_hex_nibble(int c)
    __attribute__((alias("gj_hex_nibble")));
