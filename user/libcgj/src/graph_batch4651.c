/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4651: ordered compare of two uint32_t values.
 *
 * Surface (unique symbols):
 *   int gj_u32_cmp_u(uint32_t a, uint32_t b);
 *     - Compare a and b as unsigned 32-bit integers. Returns -1 if
 *       a < b, 0 if a == b, and 1 if a > b. Strict trichotomy; no
 *       raw subtraction (avoids wrap surprises in callers).
 *   int __gj_u32_cmp_u  (alias)
 *   __libcgj_batch4651_marker = "libcgj-batch4651"
 *
 * Exclusive continuum CREATE-ONLY (4651-4660: u32_cmp_u, u64_cmp_u,
 * i32_cmp_s, u32_eq_u, u32_ne_u, u32_lt_u, u32_le_u, u32_gt_u,
 * u32_ge_u, batch_id_4660). Distinct from gj_timespec_cmp_u
 * (batch2263), gj_semver_cmp_triple (batch1794), and gj_uuid_cmp_u
 * (batch2576) — unique gj_u32_cmp_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4651_marker[] = "libcgj-batch4651";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b4651_cmp(uint32_t u32A, uint32_t u32B)
{
	if (u32A < u32B) {
		return -1;
	}
	if (u32A > u32B) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_cmp_u - ordered compare of two uint32_t values.
 *
 * a: left-hand unsigned 32-bit operand
 * b: right-hand unsigned 32-bit operand
 *
 * Returns strictly -1, 0, or 1 (trichotomy). Self-contained; no parent
 * wires.
 */
int
gj_u32_cmp_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b4651_cmp(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u32_cmp_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_u32_cmp_u")));
