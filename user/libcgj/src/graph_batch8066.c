/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8066: memcmp order is-greater predicate stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_memcmp_order_is_gt_u_8066(int32_t r);
 *     - Return 1 if r > 0 (greater-than order), else 0.
 *   uint32_t __gj_memcmp_order_is_gt_u_8066  (alias)
 *   __libcgj_batch8066_marker = "libcgj-batch8066"
 *
 * Exclusive continuum CREATE-ONLY (8061-8070: memcmp order stubs —
 * eq_id, lt_id, gt_id, is_eq, is_lt, is_gt, is_ne, sign_norm,
 * u8_asc, continuum + batch_id_8070). Unique
 * gj_memcmp_order_is_gt_u_8066 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8066_marker[] = "libcgj-batch8066";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8066_is_gt(int32_t i32R)
{
	if (i32R > 0)
		return 1u;
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_memcmp_order_is_gt_u_8066 - 1 if memcmp-style result is greater-than.
 *
 * r: raw memcmp / compare return value
 *
 * Returns 1 when r > 0, else 0. Soft greater-than gate on order; does
 * not call libc memcmp. No parent wires.
 */
uint32_t
gj_memcmp_order_is_gt_u_8066(int32_t i32R)
{
	(void)NULL;
	return b8066_is_gt(i32R);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_memcmp_order_is_gt_u_8066(int32_t i32R)
    __attribute__((alias("gj_memcmp_order_is_gt_u_8066")));
