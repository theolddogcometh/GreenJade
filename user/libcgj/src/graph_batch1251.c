/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1251: constant-time select between two uint64_t.
 *
 * Surface (unique symbols):
 *   uint64_t gj_ct_select_u64(int cond, uint64_t a, uint64_t b);
 *     — Branch-free: cond != 0 → a, else b.
 *   uint64_t __gj_ct_select_u64  (alias)
 *   __libcgj_batch1251_marker = "libcgj-batch1251"
 *
 * Distinct from gj_select_u32 (batch120) — unique u64 surface.
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1251_marker[] = "libcgj-batch1251";

/*
 * gj_ct_select_u64 — branch-free select between two uint64_t values.
 *
 * cond != 0 → a; cond == 0 → b. Mask derived from cond without a
 * data-dependent branch on a/b.
 */
uint64_t
gj_ct_select_u64(int nCond, uint64_t u64A, uint64_t u64B)
{
	uint64_t u64C;
	uint64_t u64Mask;

	u64C = (uint64_t)(nCond != 0);
	u64Mask = 0ull - u64C;

	return (u64A & u64Mask) | (u64B & ~u64Mask);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_ct_select_u64(int nCond, uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_ct_select_u64")));
