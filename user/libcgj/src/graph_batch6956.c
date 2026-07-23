/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6956: mallocate M_CHECK_ACTION flag id stub.
 *
 * Surface (unique symbols):
 *   int gj_mallocate_check_action_id_6956(void);
 *     - Returns the soft mallopt param id for M_CHECK_ACTION (-5).
 *       Catalog constant only; does not call mallopt or touch the heap.
 *   int __gj_mallocate_check_action_id_6956  (alias)
 *   __libcgj_batch6956_marker = "libcgj-batch6956"
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

const char __libcgj_batch6956_marker[] = "libcgj-batch6956";

/* Soft mallopt param id: M_CHECK_ACTION (glibc-shaped). */
#define B6956_M_CHECK_ACTION  (-5)

/* ---- freestanding helpers ---------------------------------------------- */

static int
b6956_check_action_id(void)
{
	return B6956_M_CHECK_ACTION;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mallocate_check_action_id_6956 - soft M_CHECK_ACTION param id.
 *
 * Always returns -5. Pure catalog stub; no heap side effects.
 * No parent wires.
 */
int
gj_mallocate_check_action_id_6956(void)
{
	(void)NULL;
	return b6956_check_action_id();
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_mallocate_check_action_id_6956(void)
    __attribute__((alias("gj_mallocate_check_action_id_6956")));
