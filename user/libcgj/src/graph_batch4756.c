/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4756: pure-u64 bump mark (save cursor) (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_bump_mark_u(uint64_t cur);
 *     - Capture the current watermark for a later rewind: return cur.
 *   uint64_t __gj_bump_mark_u  (alias)
 *   __libcgj_batch4756_marker = "libcgj-batch4756"
 *
 * Exclusive continuum CREATE-ONLY (4751-4760: bump_alloc_u,
 * bump_align_u, bump_remaining_u, bump_contains_u, bump_reset_u,
 * bump_mark_u, bump_rewind_u, bump_used_u, bump_ok_u, batch_id_4760).
 * Unique gj_bump_mark_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4756_marker[] = "libcgj-batch4756";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b4756_mark(uint64_t u64Cur)
{
	return u64Cur;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bump_mark_u - save a bump watermark for later rewind.
 *
 * cur: current bump watermark
 *
 * Returns cur (the mark). Pair with gj_bump_rewind_u. Pure identity.
 * No parent wires.
 */
uint64_t
gj_bump_mark_u(uint64_t u64Cur)
{
	(void)NULL;
	return b4756_mark(u64Cur);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_bump_mark_u(uint64_t u64Cur)
    __attribute__((alias("gj_bump_mark_u")));
