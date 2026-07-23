/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1524: freestanding arena watermark reset
 * (caller-provided buffer state).
 *
 * Surface (unique symbols for the 1521–1530 exclusive helper set):
 *   void gj_arena_reset_used(size_t *used);
 *     — Set *used = 0 (reclaim all prior bump allocations that used this
 *       watermark). NULL used is a no-op. Does not touch base/cap.
 *   void __gj_arena_reset_used  (alias)
 *   __libcgj_batch1524_marker = "libcgj-batch1524"
 *
 * Note: batch176 exports gj_arena_reset_used(gj_arena_t *) with a different
 * signature but the same symbol name. Do not link this TU into the same
 * image as batch176; for co-link use gj_arena_reset_used_offset (batch971).
 * This file is freestanding-compile exclusive with the 1521–1530 set.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1524_marker[] = "libcgj-batch1524";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_arena_reset_used — clear a bump/arena used watermark (caller buffer state).
 *
 * pUsed: address of the size_t watermark used by gj_arena_bump_alloc /
 *        gj_arena_can_alloc style APIs.
 *
 * Sets *pUsed = 0. NULL pUsed is ignored. Does not free or touch the
 * underlying buffer (caller retains base/cap).
 */
void
gj_arena_reset_used(size_t *pUsed)
{
	if (pUsed == NULL) {
		return;
	}
	*pUsed = 0u;
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_arena_reset_used(size_t *pUsed)
    __attribute__((alias("gj_arena_reset_used")));
