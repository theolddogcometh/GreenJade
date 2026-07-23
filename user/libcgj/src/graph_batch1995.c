/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1995: soft smoke-gate predicate.
 *
 * Surface (unique symbols):
 *   int gj_smoke_gate_soft_p(int code);
 *     — Soft product smoke gate: return 1 if code is in the soft band
 *       [0, 3] (PASS / PASS-OFFLINE / NOT-TRIED / soft-warn style
 *       codes), else 0. Negatives and codes > 3 fail soft gate.
 *   int __gj_smoke_gate_soft_p  (alias)
 *   __libcgj_batch1995_marker = "libcgj-batch1995"
 *
 * Product bar helpers exclusive pure-data wave (1991–2000). Clean-room
 * freestanding pure C (integer only). Compiles with -ffreestanding
 * -msse2 -Wall -Wextra -Werror. No malloc, no errno, no libc. No
 * third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1995_marker[] = "libcgj-batch1995";

/* Soft band: 0..3 inclusive. */
#define B1995_SOFT_MAX  3

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if code is in soft band [0, B1995_SOFT_MAX]. */
static int
b1995_soft_p(int nCode)
{
	if (nCode < 0) {
		return 0;
	}
	if (nCode > B1995_SOFT_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_gate_soft_p — 1 iff smoke code is soft-acceptable.
 *
 * code: integer smoke result code (signed so negatives reject).
 * Soft band is [0, 3]. Distinct from gj_smoke_gate_hard_p.
 */
int
gj_smoke_gate_soft_p(int nCode)
{
	(void)NULL;
	return b1995_soft_p(nCode);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_smoke_gate_soft_p(int nCode)
    __attribute__((alias("gj_smoke_gate_soft_p")));
