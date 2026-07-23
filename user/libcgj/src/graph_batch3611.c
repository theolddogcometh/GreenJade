/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3611: uint64 subtract-with-borrow (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_sub_with_borrow_u(uint64_t a, uint64_t b,
 *                                     uint64_t borrow_in,
 *                                     uint64_t *borrow_out);
 *     - Compute wrapping a - b - (borrow_in & 1).
 *       If borrow_out != NULL, store the outgoing borrow as 0 or 1.
 *   uint64_t __gj_u64_sub_with_borrow_u  (alias)
 *   __libcgj_batch3611_marker = "libcgj-batch3611"
 *
 * Exclusive continuum CREATE-ONLY wave (3611-3620). Unique
 * gj_u64_sub_with_borrow_u surface only; no multi-def. Distinct from
 * gj_borrow_sub_u64_u (batch3022), gj_u64_sub_check (batch648),
 * gj_u64_sub_sat / gj_u64_sub_wrap. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3611_marker[] = "libcgj-batch3611";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Subtract-with-borrow without __int128:
 *   t  = a - b          (wrap); c1 = (a < b)
 *   r  = t - bin        (wrap); c2 = (t < bin)
 *   bout = c1 | c2
 */
static uint64_t
b3611_sub_with_borrow(uint64_t u64A, uint64_t u64B, uint64_t u64BorrowIn,
    uint64_t *pu64BorrowOut)
{
	uint64_t u64Bin;
	uint64_t u64T;
	uint64_t u64R;
	uint64_t u64Bout;

	u64Bin = u64BorrowIn & 1ull;
	u64T = u64A - u64B;
	u64Bout = (u64A < u64B) ? 1ull : 0ull;
	u64R = u64T - u64Bin;
	if (u64T < u64Bin) {
		u64Bout = 1ull;
	}
	if (pu64BorrowOut != NULL) {
		*pu64BorrowOut = u64Bout;
	}
	return u64R;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_sub_with_borrow_u - wrapping a - b - (borrow_in & 1); report
 * borrow.
 *
 * a: minuend
 * b: subtrahend
 * borrow_in: incoming borrow (only bit0 is used)
 * borrow_out: optional; receives 0 or 1 if non-NULL
 *
 * Returns the wrapped difference. No parent wires.
 */
uint64_t
gj_u64_sub_with_borrow_u(uint64_t u64A, uint64_t u64B, uint64_t u64BorrowIn,
    uint64_t *pu64BorrowOut)
{
	(void)NULL;
	return b3611_sub_with_borrow(u64A, u64B, u64BorrowIn, pu64BorrowOut);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_sub_with_borrow_u(uint64_t u64A, uint64_t u64B,
    uint64_t u64BorrowIn, uint64_t *pu64BorrowOut)
    __attribute__((alias("gj_u64_sub_with_borrow_u")));
