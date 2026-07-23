/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1299: red-black tree color red-hint predicate.
 *
 * Surface (unique symbols):
 *   int gj_rb_is_red_hint(int color);
 *     — Return 1 if color encodes red, else 0. Convention: non-zero is
 *       red, zero is black (including NIL/sentinel). "Hint" — does not
 *       inspect a node structure, only the color integer.
 *   int __gj_rb_is_red_hint  (alias)
 *   __libcgj_batch1299_marker = "libcgj-batch1299"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

const char __libcgj_batch1299_marker[] = "libcgj-batch1299";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rb_is_red_hint — 1 if color is non-zero (red), else 0 (black).
 */
int
gj_rb_is_red_hint(int nColor)
{
	return (nColor != 0) ? 1 : 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_rb_is_red_hint(int nColor)
    __attribute__((alias("gj_rb_is_red_hint")));
