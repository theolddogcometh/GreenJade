/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8063: memcmp order greater-than id stub.
 *
 * Surface (unique symbols):
 *   int gj_memcmp_order_gt_id_8063(void);
 *     - Return soft memcmp greater-than order id (1).
 *   int __gj_memcmp_order_gt_id_8063  (alias)
 *   __libcgj_batch8063_marker = "libcgj-batch8063"
 *
 * Exclusive continuum CREATE-ONLY (8061-8070: memcmp order stubs —
 * eq_id, lt_id, gt_id, is_eq, is_lt, is_gt, is_ne, sign_norm,
 * u8_asc, continuum + batch_id_8070). Unique
 * gj_memcmp_order_gt_id_8063 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8063_marker[] = "libcgj-batch8063";

/* Soft normalized greater-than (memcmp may return any positive). */
#define B8063_ORDER_GT  1

/* ---- freestanding helpers ---------------------------------------------- */

static int
b8063_gt_id(void)
{
	return B8063_ORDER_GT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_memcmp_order_gt_id_8063 - soft memcmp greater-than order id.
 *
 * Always returns 1. Catalog tag for the a > b path after sign
 * normalize; does not call libc memcmp. No parent wires.
 */
int
gj_memcmp_order_gt_id_8063(void)
{
	(void)NULL;
	return b8063_gt_id();
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_memcmp_order_gt_id_8063(void)
    __attribute__((alias("gj_memcmp_order_gt_id_8063")));
