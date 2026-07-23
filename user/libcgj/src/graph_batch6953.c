/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6953: mallocate M_TOP_PAD flag id stub.
 *
 * Surface (unique symbols):
 *   int gj_mallocate_top_pad_id_6953(void);
 *     - Returns the soft mallopt param id for M_TOP_PAD (-2). Catalog
 *       constant only; does not call mallopt or touch the heap.
 *   int __gj_mallocate_top_pad_id_6953  (alias)
 *   __libcgj_batch6953_marker = "libcgj-batch6953"
 *
 * Mallocate flags stubs exclusive CREATE-ONLY wave (6951-6960). Unique
 * batch-suffixed surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6953_marker[] = "libcgj-batch6953";

/* Soft mallopt param id: M_TOP_PAD (glibc-shaped). */
#define B6953_M_TOP_PAD  (-2)

/* ---- freestanding helpers ---------------------------------------------- */

static int
b6953_top_pad_id(void)
{
	return B6953_M_TOP_PAD;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mallocate_top_pad_id_6953 - soft M_TOP_PAD mallopt param id.
 *
 * Always returns -2. Pure catalog stub; no heap side effects.
 * No parent wires.
 */
int
gj_mallocate_top_pad_id_6953(void)
{
	(void)NULL;
	return b6953_top_pad_id();
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_mallocate_top_pad_id_6953(void)
    __attribute__((alias("gj_mallocate_top_pad_id_6953")));
