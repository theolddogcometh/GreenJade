/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1279: ASCII hex-digit predicate (crypto).
 *
 * Surface (unique symbols — distinct from gj_is_hex_str (batch450) and
 * isxdigit / isxdigit_l):
 *   int gj_crypto_is_hex(int ch);
 *     — Return 1 if ch is an ASCII hexadecimal digit [0-9A-Fa-f].
 *       Else 0. ch outside 0..255 → 0.
 *   int __gj_crypto_is_hex  (alias)
 *   __libcgj_batch1279_marker = "libcgj-batch1279"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

const char __libcgj_batch1279_marker[] = "libcgj-batch1279";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crypto_is_hex — 1 if ch is [0-9A-Fa-f].
 */
int
gj_crypto_is_hex(int nCh)
{
	if (nCh < 0 || nCh > 255) {
		return 0;
	}
	if (nCh >= '0' && nCh <= '9') {
		return 1;
	}
	if (nCh >= 'a' && nCh <= 'f') {
		return 1;
	}
	if (nCh >= 'A' && nCh <= 'F') {
		return 1;
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_crypto_is_hex(int nCh)
    __attribute__((alias("gj_crypto_is_hex")));
