/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4754: pure-u64 bump range membership (_u).
 *
 * Surface (unique symbols):
 *   int gj_bump_contains_u(uint64_t base, uint64_t cur, uint64_t ptr);
 *     - True (1) when ptr lies in the half-open used region [base, cur);
 *       else 0.
 *   int __gj_bump_contains_u  (alias)
 *   __libcgj_batch4754_marker = "libcgj-batch4754"
 *
 * Exclusive continuum CREATE-ONLY (4751-4760: bump_alloc_u,
 * bump_align_u, bump_remaining_u, bump_contains_u, bump_reset_u,
 * bump_mark_u, bump_rewind_u, bump_used_u, bump_ok_u, batch_id_4760).
 * Distinct from gj_slice_contains_u (batch4682) — unique
 * gj_bump_contains_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4754_marker[] = "libcgj-batch4754";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b4754_contains(uint64_t u64Base, uint64_t u64Cur, uint64_t u64Ptr)
{
	if (u64Ptr >= u64Base && u64Ptr < u64Cur) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bump_contains_u - true when ptr is in the used half-open [base, cur).
 *
 * base: arena base address/offset
 * cur:  current bump watermark (exclusive end of used region)
 * ptr:  address/offset to test
 *
 * Returns 1 when base <= ptr < cur, else 0. Empty/inverted regions
 * (cur <= base) never contain. No parent wires.
 */
int
gj_bump_contains_u(uint64_t u64Base, uint64_t u64Cur, uint64_t u64Ptr)
{
	(void)NULL;
	return b4754_contains(u64Base, u64Cur, u64Ptr);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_bump_contains_u(uint64_t u64Base, uint64_t u64Cur, uint64_t u64Ptr)
    __attribute__((alias("gj_bump_contains_u")));
