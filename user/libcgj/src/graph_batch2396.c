/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2396: dynamic smoke soft-ok predicate.
 *
 * Surface (unique symbols):
 *   int gj_dyn_smoke_ok_p(int code);
 *     - Return 1 if dyn-smoke code is product-acceptable: code is in
 *       [0, 2] (PASS, soft-warn, soft-skip). Negatives and codes > 2
 *       fail. Soft pure-data gate for cgj_dyn_smoke style results.
 *   int __gj_dyn_smoke_ok_p  (alias)
 *   __libcgj_batch2396_marker = "libcgj-batch2396"
 *
 * Milestone 2400 exclusive product helpers (2391-2400). Unique
 * gj_dyn_smoke_ok_p surface only; no multi-def. Distinct from
 * gj_smoke_green_p (batch2394 exact 0), gj_smoke_gate_soft_p
 * (batch1995 band [0,3]), and gj_smoke_band_ok (batch2296 band [0,5]).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2396_marker[] = "libcgj-batch2396";

/* Inclusive upper bound of dyn-smoke soft-ok band. */
#define B2396_DYN_MAX  2

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if code is in soft dyn-smoke band [0, B2396_DYN_MAX]. */
static int
b2396_dyn_ok(int nCode)
{
	if (nCode < 0) {
		return 0;
	}
	if (nCode > B2396_DYN_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dyn_smoke_ok_p - 1 iff dyn-smoke code is product soft-ok.
 *
 * code: integer dyn-smoke result code (signed so negatives reject).
 * Band is [0, 2]. Distinct from static smoke green (exact 0) and the
 * wider soft/band gates. Does not call libc.
 */
int
gj_dyn_smoke_ok_p(int nCode)
{
	(void)NULL;
	return b2396_dyn_ok(nCode);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_dyn_smoke_ok_p(int nCode)
    __attribute__((alias("gj_dyn_smoke_ok_p")));
