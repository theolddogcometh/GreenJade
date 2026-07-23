/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch911: JSON decimal digit predicate.
 *
 * Surface (unique symbols):
 *   int gj_json_is_digit(int c);
 *     — Return 1 if c is an ASCII decimal digit '0'..'9', else 0.
 *   int __gj_json_is_digit  (alias)
 *   __libcgj_batch911_marker = "libcgj-batch911"
 *
 * Does NOT redefine gj_json_is_ws (batch741) or other gj_json_* — avoid
 * multi-def. Digit set matches RFC 8259 number production.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

const char __libcgj_batch911_marker[] = "libcgj-batch911";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_json_is_digit — true for ASCII '0' through '9'.
 *
 * Other values (including EOF-style negatives) return 0.
 */
int
gj_json_is_digit(int c)
{
	return (c >= '0' && c <= '9') ? 1 : 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_json_is_digit(int c)
    __attribute__((alias("gj_json_is_digit")));
