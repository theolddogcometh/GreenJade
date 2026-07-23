/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4757: pure-u64 bump rewind to mark (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_bump_rewind_u(uint64_t mark, uint64_t cur);
 *     - If mark <= cur return mark (rewind watermark); else return cur
 *       (reject forward/invalid marks).
 *   uint64_t __gj_bump_rewind_u  (alias)
 *   __libcgj_batch4757_marker = "libcgj-batch4757"
 *
 * Exclusive continuum CREATE-ONLY (4751-4760: bump_alloc_u,
 * bump_align_u, bump_remaining_u, bump_contains_u, bump_reset_u,
 * bump_mark_u, bump_rewind_u, bump_used_u, bump_ok_u, batch_id_4760).
 * Unique gj_bump_rewind_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4757_marker[] = "libcgj-batch4757";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b4757_rewind(uint64_t u64Mark, uint64_t u64Cur)
{
	if (u64Mark <= u64Cur) {
		return u64Mark;
	}
	return u64Cur;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bump_rewind_u - restore a bump cursor to a prior mark.
 *
 * mark: saved watermark from gj_bump_mark_u
 * cur:  current watermark
 *
 * Returns mark when mark <= cur (valid rewind); otherwise returns cur
 * unchanged (mark is ahead of cur — refuse to advance via rewind).
 * No parent wires.
 */
uint64_t
gj_bump_rewind_u(uint64_t u64Mark, uint64_t u64Cur)
{
	(void)NULL;
	return b4757_rewind(u64Mark, u64Cur);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_bump_rewind_u(uint64_t u64Mark, uint64_t u64Cur)
    __attribute__((alias("gj_bump_rewind_u")));
