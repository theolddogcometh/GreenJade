/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4759: pure-u64 bump cursor validity (_u).
 *
 * Surface (unique symbols):
 *   int gj_bump_ok_u(uint64_t cur, uint64_t end);
 *     - True (1) when cur <= end (watermark within arena); else 0.
 *   int __gj_bump_ok_u  (alias)
 *   __libcgj_batch4759_marker = "libcgj-batch4759"
 *
 * Exclusive continuum CREATE-ONLY (4751-4760: bump_alloc_u,
 * bump_align_u, bump_remaining_u, bump_contains_u, bump_reset_u,
 * bump_mark_u, bump_rewind_u, bump_used_u, bump_ok_u, batch_id_4760).
 * Unique gj_bump_ok_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4759_marker[] = "libcgj-batch4759";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b4759_ok(uint64_t u64Cur, uint64_t u64End)
{
	if (u64Cur <= u64End) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bump_ok_u - true when the bump watermark is within the arena.
 *
 * cur: current bump watermark
 * end: exclusive arena end
 *
 * Returns 1 when cur <= end, else 0. No parent wires.
 */
int
gj_bump_ok_u(uint64_t u64Cur, uint64_t u64End)
{
	(void)NULL;
	return b4759_ok(u64Cur, u64End);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_bump_ok_u(uint64_t u64Cur, uint64_t u64End)
    __attribute__((alias("gj_bump_ok_u")));
