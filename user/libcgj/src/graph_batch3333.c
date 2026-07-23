/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3333: red-black tree red color constant
 * (_u exclusive surface).
 *
 * Surface (unique symbols):
 *   int gj_rb_color_red_u(void);
 *     - Returns the red encoding used by exclusive RB color helpers
 *       (always 1). Convention: non-zero is red, zero is black.
 *   int __gj_rb_color_red_u  (alias)
 *   __libcgj_batch3333_marker = "libcgj-batch3333"
 *
 * Exclusive continuum CREATE-ONLY (3331-3340). Distinct from
 * gj_rb_set_red (batch2343), gj_rb_is_red_p (batch2341), and
 * gj_rb_is_red_hint (batch1299) - unique gj_rb_color_red_u surface
 * only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3333_marker[] = "libcgj-batch3333";

/* Red encoding returned by this helper. */
#define B3333_RED 1

/* ---- freestanding helpers ---------------------------------------------- */

static int
b3333_red(void)
{
	return B3333_RED;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rb_color_red_u - red color constant for exclusive RB helpers.
 *
 * Always returns 1 (red). Link-time presence of this symbol tags the
 * exclusive color surface. Does not call libc. No parent wires.
 */
int
gj_rb_color_red_u(void)
{
	(void)NULL;
	return b3333_red();
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_rb_color_red_u(void)
    __attribute__((alias("gj_rb_color_red_u")));
