/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2344: freestanding red-black set-black helper
 * (post-2340 tree exclusive surface).
 *
 * Surface (unique symbols):
 *   void gj_rb_set_black(int *color);
 *     - Write black encoding into *color (value 0). Convention: zero is
 *       black, non-zero is red. NULL color pointer -> no-op.
 *   void __gj_rb_set_black  (alias)
 *   __libcgj_batch2344_marker = "libcgj-batch2344"
 *
 * Post-2340 tree exclusive wave (2341-2350). Distinct from
 * gj_rb_set_red (batch2343) - unique gj_rb_set_black surface only;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2344_marker[] = "libcgj-batch2344";

/* Black encoding written by this helper. */
#define B2344_BLACK 0

/* ---- freestanding helpers ---------------------------------------------- */

/* Store black into *pnColor when the pointer is non-NULL. */
static void
b2344_set_black(int *pnColor)
{
	if (pnColor == NULL) {
		return;
	}
	*pnColor = B2344_BLACK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rb_set_black - mark an RB color integer as black.
 *
 * color: address of color word (NULL -> no-op)
 *
 * Writes 0 (black). Does not call libc.
 */
void
gj_rb_set_black(int *pnColor)
{
	(void)NULL;
	b2344_set_black(pnColor);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_rb_set_black(int *pnColor)
    __attribute__((alias("gj_rb_set_black")));
