/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6954: mallocate M_MMAP_THRESHOLD flag id stub.
 *
 * Surface (unique symbols):
 *   int gj_mallocate_mmap_threshold_id_6954(void);
 *     - Returns the soft mallopt param id for M_MMAP_THRESHOLD (-3).
 *       Catalog constant only; does not call mallopt or touch the heap.
 *   int __gj_mallocate_mmap_threshold_id_6954  (alias)
 *   __libcgj_batch6954_marker = "libcgj-batch6954"
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

const char __libcgj_batch6954_marker[] = "libcgj-batch6954";

/* Soft mallopt param id: M_MMAP_THRESHOLD (glibc-shaped). */
#define B6954_M_MMAP_THRESHOLD  (-3)

/* ---- freestanding helpers ---------------------------------------------- */

static int
b6954_mmap_threshold_id(void)
{
	return B6954_M_MMAP_THRESHOLD;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mallocate_mmap_threshold_id_6954 - soft M_MMAP_THRESHOLD param id.
 *
 * Always returns -3. Pure catalog stub; no heap side effects.
 * No parent wires.
 */
int
gj_mallocate_mmap_threshold_id_6954(void)
{
	(void)NULL;
	return b6954_mmap_threshold_id();
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_mallocate_mmap_threshold_id_6954(void)
    __attribute__((alias("gj_mallocate_mmap_threshold_id_6954")));
