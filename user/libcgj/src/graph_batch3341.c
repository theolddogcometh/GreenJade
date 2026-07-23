/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3341: arena used-watermark reset (u-suffix).
 *
 * Surface (unique symbols):
 *   size_t gj_arena_reset_u(size_t *used);
 *     - Clear a bump/arena used watermark: store *used = 0 and return
 *       the previous value. NULL used → 0 (no-op). Soft pure-data
 *       reclaim for caller-owned buffer state.
 *   size_t __gj_arena_reset_u  (alias)
 *   __libcgj_batch3341_marker = "libcgj-batch3341"
 *
 * Milestone 3350 exclusive continuum CREATE-ONLY (3341-3350). Distinct
 * from gj_arena_reset (batch176 struct), gj_arena_reset_offset
 * (batch971 void), and gj_arena_reset_used (batch1524 void) — unique
 * gj_arena_reset_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3341_marker[] = "libcgj-batch3341";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b3341_reset(size_t *pUsed)
{
	size_t uPrev;

	if (pUsed == NULL) {
		return 0u;
	}
	uPrev = *pUsed;
	*pUsed = 0u;
	return uPrev;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_arena_reset_u - clear a bump/arena used watermark; return prior value.
 *
 * used: address of the size_t watermark advanced by bump/alloc helpers.
 *
 * Sets *used = 0 and returns the previous *used. NULL used → 0.
 * Does not touch the underlying buffer (caller retains base/cap).
 * No parent wires.
 */
size_t
gj_arena_reset_u(size_t *pUsed)
{
	(void)NULL;
	return b3341_reset(pUsed);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_arena_reset_u(size_t *pUsed)
    __attribute__((alias("gj_arena_reset_u")));
