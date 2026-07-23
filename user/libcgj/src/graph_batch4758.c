/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4758: pure-u64 bump used-byte count (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_bump_used_u(uint64_t base, uint64_t cur);
 *     - Bytes consumed: cur - base when cur >= base, else 0.
 *   uint64_t __gj_bump_used_u  (alias)
 *   __libcgj_batch4758_marker = "libcgj-batch4758"
 *
 * Exclusive continuum CREATE-ONLY (4751-4760: bump_alloc_u,
 * bump_align_u, bump_remaining_u, bump_contains_u, bump_reset_u,
 * bump_mark_u, bump_rewind_u, bump_used_u, bump_ok_u, batch_id_4760).
 * Distinct from gj_bump_remaining_u (batch4753) — unique gj_bump_used_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4758_marker[] = "libcgj-batch4758";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b4758_used(uint64_t u64Base, uint64_t u64Cur)
{
	if (u64Cur >= u64Base) {
		return u64Cur - u64Base;
	}
	return 0ULL;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bump_used_u - bytes consumed from base up to cur.
 *
 * base: arena base address/offset
 * cur:  current bump watermark
 *
 * Returns cur - base when cur >= base, else 0 (inverted). No parent
 * wires.
 */
uint64_t
gj_bump_used_u(uint64_t u64Base, uint64_t u64Cur)
{
	(void)NULL;
	return b4758_used(u64Base, u64Cur);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_bump_used_u(uint64_t u64Base, uint64_t u64Cur)
    __attribute__((alias("gj_bump_used_u")));
