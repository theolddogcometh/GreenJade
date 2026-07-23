/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1996: hard smoke-gate predicate.
 *
 * Surface (unique symbols):
 *   int gj_smoke_gate_hard_p(int code);
 *     — Hard product smoke gate: return 1 only if code == 0 (strict
 *       PASS). All other codes (including soft band 1..3) fail.
 *   int __gj_smoke_gate_hard_p  (alias)
 *   __libcgj_batch1996_marker = "libcgj-batch1996"
 *
 * Product bar helpers exclusive pure-data wave (1991–2000). Clean-room
 * freestanding pure C (integer only). Compiles with -ffreestanding
 * -msse2 -Wall -Wextra -Werror. No malloc, no errno, no libc. No
 * third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1996_marker[] = "libcgj-batch1996";

/* Hard pass code. */
#define B1996_HARD_PASS  0

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 only if code is the hard PASS value. */
static int
b1996_hard_p(int nCode)
{
	return (nCode == B1996_HARD_PASS) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_gate_hard_p — 1 iff smoke code is strict PASS (0).
 *
 * code: integer smoke result code.
 * Distinct from gj_smoke_gate_soft_p (soft band [0,3]).
 */
int
gj_smoke_gate_hard_p(int nCode)
{
	(void)NULL;
	return b1996_hard_p(nCode);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_smoke_gate_hard_p(int nCode)
    __attribute__((alias("gj_smoke_gate_hard_p")));
