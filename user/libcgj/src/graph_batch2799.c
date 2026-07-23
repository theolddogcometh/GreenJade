/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2799: paired dyn smoke soft-ok (exclusive _u).
 *
 * Surface (unique symbols):
 *   int gj_dyn_ok_pair_u(uint32_t code_a, uint32_t code_b);
 *     - Return 1 if both dyn-smoke codes are soft-ok: each code is
 *       in [0, 2] (PASS, soft-warn, soft-skip). Codes > 2 fail.
 *       Unsigned pure-data form for paired cgj_dyn_smoke results.
 *   int __gj_dyn_ok_pair_u  (alias)
 *   __libcgj_batch2799_marker = "libcgj-batch2799"
 *
 * Milestone 2800 exclusive byte/hex helpers (2791-2800). Unique
 * gj_dyn_ok_pair_u surface only; no multi-def. Distinct from
 * gj_dyn_green_pair_p (batch2694 signed), gj_smoke_ok_pair_u
 * (batch2798 exact 0 pair), gj_dyn_pass_p (batch2496 single), and
 * gj_dyn_smoke_ok_p (batch2396).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2799_marker[] = "libcgj-batch2799";

/* Inclusive upper bound of dyn-smoke soft-ok band. */
#define B2799_DYN_OK_MAX  2u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if one dyn code is in soft-ok band [0, B2799_DYN_OK_MAX]. */
static int
b2799_one_ok(uint32_t uCode)
{
	if (uCode > B2799_DYN_OK_MAX) {
		return 0;
	}
	return 1;
}

/* 1 if both dyn codes are soft-ok. */
static int
b2799_ok_pair(uint32_t uCodeA, uint32_t uCodeB)
{
	if (b2799_one_ok(uCodeA) == 0) {
		return 0;
	}
	if (b2799_one_ok(uCodeB) == 0) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_ok_pair_u - 1 iff both dyn-smoke codes are soft-ok.
 *
 * code_a: first dyn-smoke result (unsigned)
 * code_b: second dyn-smoke result (unsigned)
 *
 * Soft-ok band is [0, 2] for each code. Distinct from static ok
 * pair (exact 0) and single-code dyn pass. Does not call libc.
 */
int
gj_dyn_ok_pair_u(uint32_t uCodeA, uint32_t uCodeB)
{
	(void)NULL;
	return b2799_ok_pair(uCodeA, uCodeB);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_dyn_ok_pair_u(uint32_t uCodeA, uint32_t uCodeB)
    __attribute__((alias("gj_dyn_ok_pair_u")));
