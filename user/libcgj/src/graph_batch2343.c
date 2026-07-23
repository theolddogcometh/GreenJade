/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2343: freestanding red-black set-red helper
 * (post-2340 tree exclusive surface).
 *
 * Surface (unique symbols):
 *   void gj_rb_set_red(int *color);
 *     - Write red encoding into *color (value 1). Convention: non-zero
 *       is red, zero is black. NULL color pointer -> no-op.
 *   void __gj_rb_set_red  (alias)
 *   __libcgj_batch2343_marker = "libcgj-batch2343"
 *
 * Post-2340 tree exclusive wave (2341-2350). Distinct from
 * gj_rb_is_red_p (batch2341) - unique gj_rb_set_red surface only;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2343_marker[] = "libcgj-batch2343";

/* Red encoding written by this helper. */
#define B2343_RED 1

/* ---- freestanding helpers ---------------------------------------------- */

/* Store red into *pnColor when the pointer is non-NULL. */
static void
b2343_set_red(int *pnColor)
{
	if (pnColor == NULL) {
		return;
	}
	*pnColor = B2343_RED;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rb_set_red - mark an RB color integer as red.
 *
 * color: address of color word (NULL -> no-op)
 *
 * Writes 1 (red). Does not call libc.
 */
void
gj_rb_set_red(int *pnColor)
{
	(void)NULL;
	b2343_set_red(pnColor);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_rb_set_red(int *pnColor)
    __attribute__((alias("gj_rb_set_red")));
