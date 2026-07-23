/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4752: pure-u64 bump cursor align-up (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_bump_align_u(uint64_t cur, uint64_t align);
 *     - Round cur up to the next multiple of align when align is a
 *       power of two. align 0 or 1 leaves cur unchanged. Non-power-of-
 *       two align leaves cur unchanged. Overflow of the align-up math
 *       returns 0.
 *   uint64_t __gj_bump_align_u  (alias)
 *   __libcgj_batch4752_marker = "libcgj-batch4752"
 *
 * Exclusive continuum CREATE-ONLY (4751-4760: bump_alloc_u,
 * bump_align_u, bump_remaining_u, bump_contains_u, bump_reset_u,
 * bump_mark_u, bump_rewind_u, bump_used_u, bump_ok_u, batch_id_4760).
 * Distinct from gj_bump_alloc (batch282) internal align; unique
 * gj_bump_align_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4752_marker[] = "libcgj-batch4752";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b4752_is_pow2(uint64_t u64A)
{
	if (u64A == 0ULL) {
		return 0;
	}
	return ((u64A & (u64A - 1ULL)) == 0ULL) ? 1 : 0;
}

static uint64_t
b4752_align(uint64_t u64Cur, uint64_t u64Align)
{
	uint64_t u64Mask;
	uint64_t u64Adj;

	if (u64Align <= 1ULL) {
		return u64Cur;
	}
	if (!b4752_is_pow2(u64Align)) {
		return u64Cur;
	}
	u64Mask = u64Align - 1ULL;
	u64Adj = (u64Cur + u64Mask) & ~u64Mask;
	/* overflow: (cur + mask) wrapped */
	if (u64Adj < u64Cur) {
		return 0ULL;
	}
	return u64Adj;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bump_align_u - round a u64 bump cursor up to alignment.
 *
 * cur:   current bump watermark
 * align: alignment in bytes (0/1 = no-op; power of two = align-up;
 *        other values leave cur unchanged)
 *
 * Returns aligned cursor, or 0 if align-up overflows uint64_t.
 * No parent wires.
 */
uint64_t
gj_bump_align_u(uint64_t u64Cur, uint64_t u64Align)
{
	(void)NULL;
	return b4752_align(u64Cur, u64Align);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_bump_align_u(uint64_t u64Cur, uint64_t u64Align)
    __attribute__((alias("gj_bump_align_u")));
