/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4751: pure-u64 bump allocate advance (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_bump_alloc_u(uint64_t cur, uint64_t end, uint64_t size);
 *     - Pure functional bump: if cur + size fits in [cur, end] without
 *       overflow and cur <= end, return the new cursor (cur + size);
 *       else return 0 (fail). Caller retains the pre-call cur as the
 *       allocation start address.
 *   uint64_t __gj_bump_alloc_u  (alias)
 *   __libcgj_batch4751_marker = "libcgj-batch4751"
 *
 * Exclusive continuum CREATE-ONLY (4751-4760: bump_alloc_u,
 * bump_align_u, bump_remaining_u, bump_contains_u, bump_reset_u,
 * bump_mark_u, bump_rewind_u, bump_used_u, bump_ok_u, batch_id_4760).
 * Distinct from gj_bump_alloc (batch282) which mutates *used over a
 * pointer buffer — unique gj_bump_alloc_u u64 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4751_marker[] = "libcgj-batch4751";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b4751_alloc(uint64_t u64Cur, uint64_t u64End, uint64_t u64Size)
{
	uint64_t u64Next;

	if (u64Cur > u64End) {
		return 0ULL;
	}
	if (u64Size > (u64End - u64Cur)) {
		return 0ULL;
	}
	u64Next = u64Cur + u64Size;
	return u64Next;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bump_alloc_u - advance a u64 bump cursor by size within [cur, end].
 *
 * cur:  current bump watermark (address/offset as uint64_t)
 * end:  exclusive end of the arena
 * size: bytes to reserve
 *
 * Returns new cursor (cur + size) on success, or 0 on fail (cur > end,
 * size does not fit, or overflow). size == 0 succeeds with return cur
 * when cur <= end (including cur == end). No parent wires.
 */
uint64_t
gj_bump_alloc_u(uint64_t u64Cur, uint64_t u64End, uint64_t u64Size)
{
	(void)NULL;
	return b4751_alloc(u64Cur, u64End, u64Size);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_bump_alloc_u(uint64_t u64Cur, uint64_t u64End, uint64_t u64Size)
    __attribute__((alias("gj_bump_alloc_u")));
