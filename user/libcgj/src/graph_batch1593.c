/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1593: JSON decimal digit predicate.
 *
 * Surface (unique symbols):
 *   int gj_json_is_digit_u(int c);
 *     — Return 1 if c is an ASCII decimal digit '0'..'9', else 0.
 *   int __gj_json_is_digit_u  (alias)
 *   __libcgj_batch1593_marker = "libcgj-batch1593"
 *
 * Digit set matches RFC 8259 number production.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

const char __libcgj_batch1593_marker[] = "libcgj-batch1593";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1593_is_digit(int nC)
{
	return (nC >= '0' && nC <= '9') ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_json_is_digit_u — true for ASCII '0' through '9'.
 *
 * Other values (including EOF-style negatives) return 0.
 */
int
gj_json_is_digit_u(int nC)
{
	return b1593_is_digit(nC);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_json_is_digit_u(int nC)
    __attribute__((alias("gj_json_is_digit_u")));
