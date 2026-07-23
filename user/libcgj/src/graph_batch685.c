/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch685: freestanding ASCII hex-digit predicate.
 *
 * Surface (unique symbols):
 *   int gj_is_hex_digit(int c);
 *     — Return 1 if c is an ASCII hexadecimal digit in
 *       '0'..'9', 'a'..'f', or 'A'..'F'; else 0.
 *   int __gj_is_hex_digit  (alias)
 *   __libcgj_batch685_marker = "libcgj-batch685"
 *
 * Distinct from isxdigit (ctype.c) and gj_is_hex_str / b450_is_hex_digit
 * (batch450 string check). Unique gj_ names only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

const char __libcgj_batch685_marker[] = "libcgj-batch685";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_is_hex_digit — 1 iff c is in [0-9a-fA-F].
 *
 * Edge cases:
 *   '0'..'9' / 'a'..'f' / 'A'..'F' → 1
 *   'g' / 'G' / ' ' / EOF(-1) / any other → 0
 */
int
gj_is_hex_digit(int c)
{
	if (c >= '0' && c <= '9') {
		return 1;
	}
	if (c >= 'a' && c <= 'f') {
		return 1;
	}
	if (c >= 'A' && c <= 'F') {
		return 1;
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_is_hex_digit(int c)
    __attribute__((alias("gj_is_hex_digit")));
