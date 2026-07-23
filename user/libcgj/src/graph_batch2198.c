/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2198: soft-skip smoke result predicate.
 *
 * Surface (unique symbols):
 *   int gj_smoke_soft_skip_p(int code);
 *     - Return 1 if code is the soft-skip / NOT-TRIED style smoke
 *       result (exactly 2), else 0. Distinct from gj_smoke_gate_soft_p
 *       (batch1995 soft band [0,3] accept gate).
 *   int __gj_smoke_soft_skip_p  (alias)
 *   __libcgj_batch2198_marker = "libcgj-batch2198"
 *
 * Milestone 2200 exclusive product helpers (2191-2200). Unique
 * gj_smoke_soft_skip_p surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2198_marker[] = "libcgj-batch2198";

/* Soft-skip / not-tried product smoke code. */
#define B2198_SOFT_SKIP  2

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 only if code is the soft-skip sentinel. */
static int
b2198_soft_skip_p(int nCode)
{
	return (nCode == B2198_SOFT_SKIP) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_soft_skip_p - 1 iff smoke code means soft skip (not tried).
 *
 * code: integer smoke result code.
 * Soft-skip sentinel is exactly 2. PASS (0), offline (1), soft-warn (3)
 * and hard fails are not soft skips. Does not call libc.
 */
int
gj_smoke_soft_skip_p(int nCode)
{
	(void)NULL;
	return b2198_soft_skip_p(nCode);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_smoke_soft_skip_p(int nCode)
    __attribute__((alias("gj_smoke_soft_skip_p")));
