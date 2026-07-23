/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4654: equality predicate for two uint32_t.
 *
 * Surface (unique symbols):
 *   int gj_u32_eq_u(uint32_t a, uint32_t b);
 *     - Return 1 if a equals b, else 0. Pure scalar equality; no
 *       ordering.
 *   int __gj_u32_eq_u  (alias)
 *   __libcgj_batch4654_marker = "libcgj-batch4654"
 *
 * Exclusive continuum CREATE-ONLY (4651-4660: u32_cmp_u, u64_cmp_u,
 * i32_cmp_s, u32_eq_u, u32_ne_u, u32_lt_u, u32_le_u, u32_gt_u,
 * u32_ge_u, batch_id_4660). Distinct from gj_u32_cmp_u (batch4651)
 * and gj_str_eq_u (batch2622) — unique gj_u32_eq_u surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4654_marker[] = "libcgj-batch4654";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b4654_eq(uint32_t u32A, uint32_t u32B)
{
	return (u32A == u32B) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_eq_u - 1 if two uint32_t values are equal, else 0.
 *
 * a: left-hand unsigned 32-bit operand
 * b: right-hand unsigned 32-bit operand
 *
 * Returns 1 on equality, 0 otherwise. Self-contained; no parent wires.
 */
int
gj_u32_eq_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b4654_eq(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u32_eq_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_u32_eq_u")));
