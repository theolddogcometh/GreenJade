/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3334: red-black tree black color constant
 * (_u exclusive surface).
 *
 * Surface (unique symbols):
 *   int gj_rb_color_black_u(void);
 *     - Returns the black encoding used by exclusive RB color helpers
 *       (always 0). Convention: non-zero is red, zero is black.
 *   int __gj_rb_color_black_u  (alias)
 *   __libcgj_batch3334_marker = "libcgj-batch3334"
 *
 * Exclusive continuum CREATE-ONLY (3331-3340). Distinct from
 * gj_rb_set_black (batch2344), gj_rb_is_black_p (batch2342), and
 * gj_rb_color_red_u (batch3333) - unique gj_rb_color_black_u surface
 * only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3334_marker[] = "libcgj-batch3334";

/* Black encoding returned by this helper. */
#define B3334_BLACK 0

/* ---- freestanding helpers ---------------------------------------------- */

static int
b3334_black(void)
{
	return B3334_BLACK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rb_color_black_u - black color constant for exclusive RB helpers.
 *
 * Always returns 0 (black). Link-time presence of this symbol tags the
 * exclusive color surface. Does not call libc. No parent wires.
 */
int
gj_rb_color_black_u(void)
{
	(void)NULL;
	return b3334_black();
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_rb_color_black_u(void)
    __attribute__((alias("gj_rb_color_black_u")));
