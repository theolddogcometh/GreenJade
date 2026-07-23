/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2496: dynamic smoke soft-pass predicate.
 *
 * Surface (unique symbols):
 *   int gj_dyn_pass_p(int code);
 *     - Return 1 if dyn-smoke code is product soft-pass: code is in
 *       [0, 2] (PASS, soft-warn, soft-skip). Negatives and codes > 2
 *       fail. Soft pure-data gate for cgj_dyn_smoke style results.
 *   int __gj_dyn_pass_p  (alias)
 *   __libcgj_batch2496_marker = "libcgj-batch2496"
 *
 * Milestone 2500 exclusive product helpers (2491-2500). Unique
 * gj_dyn_pass_p surface only; no multi-def. Distinct from
 * gj_dyn_smoke_ok_p (batch2396), gj_smoke_pass_p (batch2494 exact 0),
 * gj_smoke_gate_soft_p (batch1995 band [0,3]), and gj_smoke_band_ok
 * (batch2296 band [0,5]).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2496_marker[] = "libcgj-batch2496";

/* Inclusive upper bound of dyn-smoke soft-pass band. */
#define B2496_DYN_MAX  2

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if code is in soft dyn-smoke band [0, B2496_DYN_MAX]. */
static int
b2496_dyn_pass(int nCode)
{
	if (nCode < 0) {
		return 0;
	}
	if (nCode > B2496_DYN_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_pass_p - 1 iff dyn-smoke code is product soft-pass.
 *
 * code: integer dyn-smoke result code (signed so negatives reject).
 * Band is [0, 2]. Distinct from static smoke pass (exact 0) and the
 * wider soft/band gates. Does not call libc.
 */
int
gj_dyn_pass_p(int nCode)
{
	(void)NULL;
	return b2496_dyn_pass(nCode);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_dyn_pass_p(int nCode)
    __attribute__((alias("gj_dyn_pass_p")));
