/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1511: constant-time select between two uint64_t.
 *
 * Surface (unique symbols):
 *   uint64_t gj_ct_cmov_u64(int cond, uint64_t a, uint64_t b);
 *     — Branch-free: cond != 0 → a, else b.
 *   uint64_t __gj_ct_cmov_u64  (alias)
 *   __libcgj_batch1511_marker = "libcgj-batch1511"
 *
 * Avoid multi-def: gj_ct_select_u64 / __gj_ct_select_u64 live in
 * graph_batch1251.c. This TU exports only the cmov-named surface.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No real crypto copied.
 */

#include <stdint.h>

const char __libcgj_batch1511_marker[] = "libcgj-batch1511";

/*
 * gj_ct_cmov_u64 — branch-free select between two uint64_t values.
 *
 * cond != 0 → a; cond == 0 → b. Mask derived from cond without a
 * data-dependent branch on a/b.
 */
uint64_t
gj_ct_cmov_u64(int nCond, uint64_t u64A, uint64_t u64B)
{
	uint64_t u64C;
	uint64_t u64Mask;

	u64C = (uint64_t)(nCond != 0);
	u64Mask = 0ull - u64C;

	return (u64A & u64Mask) | (u64B & ~u64Mask);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_ct_cmov_u64(int nCond, uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_ct_cmov_u64")));
