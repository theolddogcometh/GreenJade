/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6952: mallocate M_TRIM_THRESHOLD flag id stub.
 *
 * Surface (unique symbols):
 *   int gj_mallocate_trim_threshold_id_6952(void);
 *     - Returns the soft mallopt param id for M_TRIM_THRESHOLD (-1).
 *       Catalog constant only; does not call mallopt or touch the heap.
 *   int __gj_mallocate_trim_threshold_id_6952  (alias)
 *   __libcgj_batch6952_marker = "libcgj-batch6952"
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

const char __libcgj_batch6952_marker[] = "libcgj-batch6952";

/* Soft mallopt param id: M_TRIM_THRESHOLD (glibc-shaped). */
#define B6952_M_TRIM_THRESHOLD  (-1)

/* ---- freestanding helpers ---------------------------------------------- */

static int
b6952_trim_threshold_id(void)
{
	return B6952_M_TRIM_THRESHOLD;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mallocate_trim_threshold_id_6952 - soft M_TRIM_THRESHOLD param id.
 *
 * Always returns -1. Pure catalog stub; no heap side effects.
 * No parent wires.
 */
int
gj_mallocate_trim_threshold_id_6952(void)
{
	(void)NULL;
	return b6952_trim_threshold_id();
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_mallocate_trim_threshold_id_6952(void)
    __attribute__((alias("gj_mallocate_trim_threshold_id_6952")));
