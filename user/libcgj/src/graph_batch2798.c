/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2798: paired static smoke ok (exclusive _u).
 *
 * Surface (unique symbols):
 *   int gj_smoke_ok_pair_u(uint32_t code_a, uint32_t code_b);
 *     - Return 1 if both static smoke codes are strict ok PASS
 *       (exactly 0), else 0. Unsigned pure-data form for paired
 *       static smoke lamps (e.g. host + guest static smoke).
 *   int __gj_smoke_ok_pair_u  (alias)
 *   __libcgj_batch2798_marker = "libcgj-batch2798"
 *
 * Milestone 2800 exclusive byte/hex helpers (2791-2800). Unique
 * gj_smoke_ok_pair_u surface only; no multi-def. Distinct from
 * gj_smoke_green_pair_p (batch2693 signed), gj_smoke_pass_p
 * (batch2494 single), and gj_dyn_ok_pair_u (batch2799 soft band).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2798_marker[] = "libcgj-batch2798";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if both codes are strict ok PASS (exactly 0). */
static int
b2798_ok_pair(uint32_t uCodeA, uint32_t uCodeB)
{
	if (uCodeA != 0u) {
		return 0;
	}
	if (uCodeB != 0u) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_ok_pair_u - 1 iff both static smoke codes are ok (PASS).
 *
 * code_a: first static smoke result (unsigned)
 * code_b: second static smoke result (unsigned)
 *
 * Ok PASS is exactly 0 for both. Pair with gj_dyn_ok_pair_u for the
 * dyn soft-band counterpart. Does not call libc.
 */
int
gj_smoke_ok_pair_u(uint32_t uCodeA, uint32_t uCodeB)
{
	(void)NULL;
	return b2798_ok_pair(uCodeA, uCodeB);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_smoke_ok_pair_u(uint32_t uCodeA, uint32_t uCodeB)
    __attribute__((alias("gj_smoke_ok_pair_u")));
