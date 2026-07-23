/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8086: strcmp order greater-than predicate stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strcmp_is_gt_u_8086(int32_t cmp);
 *     - Return 1 if cmp > 0 (greater-than order), else 0.
 *   uint32_t __gj_strcmp_is_gt_u_8086  (alias)
 *   __libcgj_batch8086_marker = "libcgj-batch8086"
 *
 * Exclusive continuum CREATE-ONLY (8081-8090: strcmp order stubs —
 * ord_eq, ord_lt, ord_gt, is_eq, is_lt, is_gt, is_le, is_ge, is_ne,
 * batch_id_8090). Unique gj_strcmp_is_gt_u_8086 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8086_marker[] = "libcgj-batch8086";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8086_is_gt(int32_t i32Cmp)
{
	return (i32Cmp > 0) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strcmp_is_gt_u_8086 - 1 if a strcmp-shaped result means a > b.
 *
 * cmp: signed strcmp-style order (0 equal; <0 a<b; >0 a>b)
 *
 * Returns 1 when cmp > 0, else 0. Does not call strcmp. No parent wires.
 */
uint32_t
gj_strcmp_is_gt_u_8086(int32_t i32Cmp)
{
	(void)NULL;
	return b8086_is_gt(i32Cmp);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strcmp_is_gt_u_8086(int32_t i32Cmp)
    __attribute__((alias("gj_strcmp_is_gt_u_8086")));
