/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6959: mallocate M_ARENA_MAX flag id stub.
 *
 * Surface (unique symbols):
 *   int gj_mallocate_arena_max_id_6959(void);
 *     - Returns the soft mallopt param id for M_ARENA_MAX (-8). Catalog
 *       constant only; does not call mallopt or touch the heap.
 *   int __gj_mallocate_arena_max_id_6959  (alias)
 *   __libcgj_batch6959_marker = "libcgj-batch6959"
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

const char __libcgj_batch6959_marker[] = "libcgj-batch6959";

/* Soft mallopt param id: M_ARENA_MAX (glibc-shaped). */
#define B6959_M_ARENA_MAX  (-8)

/* ---- freestanding helpers ---------------------------------------------- */

static int
b6959_arena_max_id(void)
{
	return B6959_M_ARENA_MAX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mallocate_arena_max_id_6959 - soft M_ARENA_MAX mallopt param id.
 *
 * Always returns -8. Pure catalog stub; no heap side effects.
 * No parent wires.
 */
int
gj_mallocate_arena_max_id_6959(void)
{
	(void)NULL;
	return b6959_arena_max_id();
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_mallocate_arena_max_id_6959(void)
    __attribute__((alias("gj_mallocate_arena_max_id_6959")));
