/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2094: freestanding sequence gap predicate.
 *
 * Surface (unique symbols):
 *   int gj_seq_gap_p(uint32_t prev, uint32_t curr);
 *     - Return 1 if curr is not the modular successor of prev
 *       (curr != prev + 1 with wrap), else 0. Detects dropped or
 *       reordered sequence numbers between a stored prev and an
 *       observed curr.
 *   int __gj_seq_gap_p  (alias)
 *   __libcgj_batch2094_marker = "libcgj-batch2094"
 *
 * Post-2000 id exclusive wave (2091-2100). Unique gj_seq_gap_p surface
 * only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2094_marker[] = "libcgj-batch2094";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2094_gap_p(uint32_t u32Prev, uint32_t u32Curr)
{
	uint32_t u32Next;

	u32Next = u32Prev + 1u;
	return u32Curr != u32Next ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_seq_gap_p - true if curr is not the modular next of prev.
 *
 * prev: last accepted sequence number
 * curr: newly observed sequence number
 *
 * Returns 1 when curr != prev + 1 (unsigned wrap), else 0. Equal values
 * count as a gap (duplicate is not the next step). Does not call libc.
 */
int
gj_seq_gap_p(uint32_t u32Prev, uint32_t u32Curr)
{
	(void)NULL;
	return b2094_gap_p(u32Prev, u32Curr);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_seq_gap_p(uint32_t u32Prev, uint32_t u32Curr)
    __attribute__((alias("gj_seq_gap_p")));
