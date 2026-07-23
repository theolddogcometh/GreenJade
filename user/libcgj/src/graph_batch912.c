/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch912: JSON / JSON-string hex digit predicate.
 *
 * Surface (unique symbols):
 *   int gj_json_is_hex(int c);
 *     — Return 1 if c is an ASCII hex digit (0-9, a-f, A-F), else 0.
 *   int __gj_json_is_hex  (alias)
 *   __libcgj_batch912_marker = "libcgj-batch912"
 *
 * Used for \uXXXX escape validation (RFC 8259 §7). Does NOT redefine
 * decimal-digit or whitespace JSON predicates — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

const char __libcgj_batch912_marker[] = "libcgj-batch912";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_json_is_hex — true for ASCII hexadecimal digits.
 *
 * Accepts 0-9, a-f, and A-F. All other values return 0.
 */
int
gj_json_is_hex(int c)
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

int __gj_json_is_hex(int c)
    __attribute__((alias("gj_json_is_hex")));
