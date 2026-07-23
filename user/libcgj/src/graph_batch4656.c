/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4656: strict less-than for two uint32_t.
 *
 * Surface (unique symbols):
 *   int gj_u32_lt_u(uint32_t a, uint32_t b);
 *     - Return 1 if a < b (unsigned), else 0.
 *   int __gj_u32_lt_u  (alias)
 *   __libcgj_batch4656_marker = "libcgj-batch4656"
 *
 * Exclusive continuum CREATE-ONLY (4651-4660: u32_cmp_u, u64_cmp_u,
 * i32_cmp_s, u32_eq_u, u32_ne_u, u32_lt_u, u32_le_u, u32_gt_u,
 * u32_ge_u, batch_id_4660). Distinct from gj_u32_cmp_u (batch4651)
 * and gj_u32_le_u (batch4657) — unique gj_u32_lt_u surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4656_marker[] = "libcgj-batch4656";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b4656_lt(uint32_t u32A, uint32_t u32B)
{
	return (u32A < u32B) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_lt_u - 1 if a is strictly less than b (unsigned), else 0.
 *
 * a: left-hand unsigned 32-bit operand
 * b: right-hand unsigned 32-bit operand
 *
 * Returns 1 when a < b, 0 otherwise. Self-contained; no parent wires.
 */
int
gj_u32_lt_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b4656_lt(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u32_lt_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_u32_lt_u")));
