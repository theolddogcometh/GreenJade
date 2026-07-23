/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8061: memcmp order equal id stub.
 *
 * Surface (unique symbols):
 *   int gj_memcmp_order_eq_id_8061(void);
 *     - Return soft memcmp equal order id (0).
 *   int __gj_memcmp_order_eq_id_8061  (alias)
 *   __libcgj_batch8061_marker = "libcgj-batch8061"
 *
 * Exclusive continuum CREATE-ONLY (8061-8070: memcmp order stubs —
 * eq_id, lt_id, gt_id, is_eq, is_lt, is_gt, is_ne, sign_norm,
 * u8_asc, continuum + batch_id_8070). Unique
 * gj_memcmp_order_eq_id_8061 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8061_marker[] = "libcgj-batch8061";

/* Soft memcmp equal: classic memcmp returns 0 when windows match. */
#define B8061_ORDER_EQ  0

/* ---- freestanding helpers ---------------------------------------------- */

static int
b8061_eq_id(void)
{
	return B8061_ORDER_EQ;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_memcmp_order_eq_id_8061 - soft memcmp equal order id.
 *
 * Always returns 0. Catalog id only (windows equal); does not call
 * libc memcmp. No parent wires.
 */
int
gj_memcmp_order_eq_id_8061(void)
{
	(void)NULL;
	return b8061_eq_id();
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_memcmp_order_eq_id_8061(void)
    __attribute__((alias("gj_memcmp_order_eq_id_8061")));
