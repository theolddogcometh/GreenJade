/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch971: reset a freestanding arena offset watermark.
 *
 * Surface (unique symbols):
 *   void gj_arena_reset_offset(size_t *off);
 *     — Set *off = 0 (reclaim all prior bump allocations that used this
 *       offset). NULL off is a no-op.
 *   void __gj_arena_reset_offset  (alias)
 *   __libcgj_batch971_marker = "libcgj-batch971"
 *
 * Distinct from gj_arena_reset (batch176, struct gj_arena_t) and
 * gj_arena_bump (batch545, which advances *off). Unique
 * gj_arena_reset_offset surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch971_marker[] = "libcgj-batch971";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_arena_reset_offset — clear a bump/arena offset watermark.
 *
 * off: address of the size_t offset used by gj_arena_bump-style APIs.
 *
 * Sets *off = 0. NULL off is ignored. Does not touch the underlying
 * buffer (caller retains base/cap).
 */
void
gj_arena_reset_offset(size_t *off)
{
	if (off == NULL) {
		return;
	}
	*off = 0u;
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_arena_reset_offset(size_t *off)
    __attribute__((alias("gj_arena_reset_offset")));
