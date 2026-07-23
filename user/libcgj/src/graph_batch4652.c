/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4652: ordered compare of two uint64_t values.
 *
 * Surface (unique symbols):
 *   int gj_u64_cmp_u(uint64_t a, uint64_t b);
 *     - Compare a and b as unsigned 64-bit integers. Returns -1 if
 *       a < b, 0 if a == b, and 1 if a > b. Strict trichotomy; no
 *       raw subtraction.
 *   int __gj_u64_cmp_u  (alias)
 *   __libcgj_batch4652_marker = "libcgj-batch4652"
 *
 * Exclusive continuum CREATE-ONLY (4651-4660: u32_cmp_u, u64_cmp_u,
 * i32_cmp_s, u32_eq_u, u32_ne_u, u32_lt_u, u32_le_u, u32_gt_u,
 * u32_ge_u, batch_id_4660). Distinct from gj_u32_cmp_u (batch4651)
 * and gj_timespec_cmp_u (batch2263) — unique gj_u64_cmp_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4652_marker[] = "libcgj-batch4652";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b4652_cmp(uint64_t u64A, uint64_t u64B)
{
	if (u64A < u64B) {
		return -1;
	}
	if (u64A > u64B) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_cmp_u - ordered compare of two uint64_t values.
 *
 * a: left-hand unsigned 64-bit operand
 * b: right-hand unsigned 64-bit operand
 *
 * Returns strictly -1, 0, or 1 (trichotomy). Self-contained; no parent
 * wires.
 */
int
gj_u64_cmp_u(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b4652_cmp(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u64_cmp_u(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_cmp_u")));
