/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3609: uint32 subtract-with-borrow (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_sub_with_borrow_u(uint32_t a, uint32_t b,
 *                                     uint32_t borrow_in,
 *                                     uint32_t *borrow_out);
 *     - Compute wrapping a - b - (borrow_in & 1).
 *       If borrow_out != NULL, store the outgoing borrow as 0 or 1.
 *   uint32_t __gj_u32_sub_with_borrow_u  (alias)
 *   __libcgj_batch3609_marker = "libcgj-batch3609"
 *
 * Exclusive continuum CREATE-ONLY wave (3601-3610). Distinct from
 * gj_borrow_sub_u32_u (batch3021) and gj_subb_u64 (batch570) — unique
 * gj_u32_sub_with_borrow_u surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3609_marker[] = "libcgj-batch3609";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Subtract-with-borrow without wide types:
 *   t  = a - b          (wrap); c1 = (a < b)
 *   r  = t - bin        (wrap); c2 = (t < bin)
 *   bout = c1 | c2
 */
static uint32_t
b3609_borrow_sub(uint32_t u32A, uint32_t u32B, uint32_t u32BorrowIn,
    uint32_t *pu32BorrowOut)
{
	uint32_t u32Bin;
	uint32_t u32T;
	uint32_t u32R;
	uint32_t u32Bout;

	u32Bin = u32BorrowIn & 1u;
	u32T = u32A - u32B;
	u32Bout = (u32A < u32B) ? 1u : 0u;
	u32R = u32T - u32Bin;
	if (u32T < u32Bin) {
		u32Bout = 1u;
	}
	if (pu32BorrowOut != NULL) {
		*pu32BorrowOut = u32Bout;
	}
	return u32R;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_sub_with_borrow_u - wrapping a - b - (borrow_in & 1); report borrow.
 *
 * a: minuend
 * b: subtrahend
 * borrow_in: incoming borrow (only bit0 is used)
 * borrow_out: optional; receives 0 or 1 if non-NULL
 *
 * Returns the wrapped difference. No parent wires.
 */
uint32_t
gj_u32_sub_with_borrow_u(uint32_t u32A, uint32_t u32B, uint32_t u32BorrowIn,
    uint32_t *pu32BorrowOut)
{
	(void)NULL;
	return b3609_borrow_sub(u32A, u32B, u32BorrowIn, pu32BorrowOut);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_sub_with_borrow_u(uint32_t u32A, uint32_t u32B,
    uint32_t u32BorrowIn, uint32_t *pu32BorrowOut)
    __attribute__((alias("gj_u32_sub_with_borrow_u")));
