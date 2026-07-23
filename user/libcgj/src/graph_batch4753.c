/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4753: pure-u64 bump remaining capacity (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_bump_remaining_u(uint64_t cur, uint64_t end);
 *     - Bytes left in [cur, end]: end - cur when end >= cur, else 0.
 *   uint64_t __gj_bump_remaining_u  (alias)
 *   __libcgj_batch4753_marker = "libcgj-batch4753"
 *
 * Exclusive continuum CREATE-ONLY (4751-4760: bump_alloc_u,
 * bump_align_u, bump_remaining_u, bump_contains_u, bump_reset_u,
 * bump_mark_u, bump_rewind_u, bump_used_u, bump_ok_u, batch_id_4760).
 * Distinct from gj_bump_available (batch972) size_t used/cap form —
 * unique gj_bump_remaining_u u64 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4753_marker[] = "libcgj-batch4753";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b4753_remaining(uint64_t u64Cur, uint64_t u64End)
{
	if (u64End >= u64Cur) {
		return u64End - u64Cur;
	}
	return 0ULL;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bump_remaining_u - bytes remaining between cur and end.
 *
 * cur: current bump watermark
 * end: exclusive arena end
 *
 * Returns end - cur when end >= cur, else 0 (inverted/overrun).
 * No parent wires.
 */
uint64_t
gj_bump_remaining_u(uint64_t u64Cur, uint64_t u64End)
{
	(void)NULL;
	return b4753_remaining(u64Cur, u64End);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_bump_remaining_u(uint64_t u64Cur, uint64_t u64End)
    __attribute__((alias("gj_bump_remaining_u")));
