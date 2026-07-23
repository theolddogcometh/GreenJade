/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2494: product smoke PASS predicate.
 *
 * Surface (unique symbols):
 *   int gj_smoke_pass_p(int code);
 *     - Return 1 if code is strict PASS (exactly 0), else 0.
 *       Soft pure-data gate for product smoke status lamps.
 *   int __gj_smoke_pass_p  (alias)
 *   __libcgj_batch2494_marker = "libcgj-batch2494"
 *
 * Milestone 2500 exclusive product helpers (2491-2500). Unique
 * gj_smoke_pass_p surface only; no multi-def. Distinct from
 * gj_smoke_green_p (batch2394), gj_smoke_gate_hard_p (batch1996),
 * gj_smoke_gate_soft_p (batch1995), and gj_smoke_band_ok (batch2296).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2494_marker[] = "libcgj-batch2494";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if code is strict PASS (exactly 0). */
static int
b2494_pass_p(int nCode)
{
	if (nCode == 0) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_pass_p - 1 iff smoke code is strict PASS.
 *
 * code: integer smoke result code (signed so non-zero rejects).
 * PASS is exactly 0. Pair with gj_smoke_fail_p for the complementary
 * fail lamp. Does not call libc.
 */
int
gj_smoke_pass_p(int nCode)
{
	(void)NULL;
	return b2494_pass_p(nCode);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_smoke_pass_p(int nCode)
    __attribute__((alias("gj_smoke_pass_p")));
