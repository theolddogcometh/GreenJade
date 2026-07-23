/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6958: mallocate M_ARENA_TEST flag id stub.
 *
 * Surface (unique symbols):
 *   int gj_mallocate_arena_test_id_6958(void);
 *     - Returns the soft mallopt param id for M_ARENA_TEST (-7).
 *       Catalog constant only; does not call mallopt or touch the heap.
 *   int __gj_mallocate_arena_test_id_6958  (alias)
 *   __libcgj_batch6958_marker = "libcgj-batch6958"
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

const char __libcgj_batch6958_marker[] = "libcgj-batch6958";

/* Soft mallopt param id: M_ARENA_TEST (glibc-shaped). */
#define B6958_M_ARENA_TEST  (-7)

/* ---- freestanding helpers ---------------------------------------------- */

static int
b6958_arena_test_id(void)
{
	return B6958_M_ARENA_TEST;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mallocate_arena_test_id_6958 - soft M_ARENA_TEST mallopt param id.
 *
 * Always returns -7. Pure catalog stub; no heap side effects.
 * No parent wires.
 */
int
gj_mallocate_arena_test_id_6958(void)
{
	(void)NULL;
	return b6958_arena_test_id();
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_mallocate_arena_test_id_6958(void)
    __attribute__((alias("gj_mallocate_arena_test_id_6958")));
