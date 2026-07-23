/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4653: ordered compare of two int32_t values.
 *
 * Surface (unique symbols):
 *   int gj_i32_cmp_s(int32_t a, int32_t b);
 *     - Compare a and b as signed 32-bit integers. Returns -1 if
 *       a < b, 0 if a == b, and 1 if a > b. Strict trichotomy; no
 *       raw subtraction (avoids INT_MIN overflow).
 *   int __gj_i32_cmp_s  (alias)
 *   __libcgj_batch4653_marker = "libcgj-batch4653"
 *
 * Exclusive continuum CREATE-ONLY (4651-4660: u32_cmp_u, u64_cmp_u,
 * i32_cmp_s, u32_eq_u, u32_ne_u, u32_lt_u, u32_le_u, u32_gt_u,
 * u32_ge_u, batch_id_4660). Distinct from gj_u32_cmp_u (batch4651)
 * and gj_u64_cmp_u (batch4652) — unique signed gj_i32_cmp_s surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4653_marker[] = "libcgj-batch4653";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b4653_cmp(int32_t i32A, int32_t i32B)
{
	if (i32A < i32B) {
		return -1;
	}
	if (i32A > i32B) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_i32_cmp_s - ordered compare of two int32_t values.
 *
 * a: left-hand signed 32-bit operand
 * b: right-hand signed 32-bit operand
 *
 * Returns strictly -1, 0, or 1 (trichotomy). Self-contained; no parent
 * wires.
 */
int
gj_i32_cmp_s(int32_t i32A, int32_t i32B)
{
	(void)NULL;
	return b4653_cmp(i32A, i32B);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_i32_cmp_s(int32_t i32A, int32_t i32B)
    __attribute__((alias("gj_i32_cmp_s")));
