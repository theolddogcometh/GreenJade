/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2342: freestanding red-black black color
 * predicate (post-2340 tree exclusive surface).
 *
 * Surface (unique symbols):
 *   int gj_rb_is_black_p(int color);
 *     - Return 1 if color encodes black, else 0. Convention: zero is
 *       black (including NIL/sentinel), non-zero is red.
 *   int __gj_rb_is_black_p  (alias)
 *   __libcgj_batch2342_marker = "libcgj-batch2342"
 *
 * Post-2340 tree exclusive wave (2341-2350). Distinct from
 * gj_rb_is_red_p (batch2341) / gj_rb_is_red_hint (batch1299) - unique
 * gj_rb_is_black_p surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2342_marker[] = "libcgj-batch2342";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 when color is zero (black). */
static int
b2342_is_black_p(int nColor)
{
	if (nColor == 0) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rb_is_black_p - true when an RB color integer encodes black.
 *
 * color: RB color word (0 = black, non-zero = red)
 *
 * Returns 1 for black, 0 for red. Does not call libc.
 */
int
gj_rb_is_black_p(int nColor)
{
	(void)NULL;
	return b2342_is_black_p(nColor);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_rb_is_black_p(int nColor)
    __attribute__((alias("gj_rb_is_black_p")));
