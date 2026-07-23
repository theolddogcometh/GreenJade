/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8062: memcmp order less-than id stub.
 *
 * Surface (unique symbols):
 *   int gj_memcmp_order_lt_id_8062(void);
 *     - Return soft memcmp less-than order id (-1).
 *   int __gj_memcmp_order_lt_id_8062  (alias)
 *   __libcgj_batch8062_marker = "libcgj-batch8062"
 *
 * Exclusive continuum CREATE-ONLY (8061-8070: memcmp order stubs —
 * eq_id, lt_id, gt_id, is_eq, is_lt, is_gt, is_ne, sign_norm,
 * u8_asc, continuum + batch_id_8070). Unique
 * gj_memcmp_order_lt_id_8062 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8062_marker[] = "libcgj-batch8062";

/* Soft normalized less-than (memcmp may return any negative). */
#define B8062_ORDER_LT  (-1)

/* ---- freestanding helpers ---------------------------------------------- */

static int
b8062_lt_id(void)
{
	return B8062_ORDER_LT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_memcmp_order_lt_id_8062 - soft memcmp less-than order id.
 *
 * Always returns -1. Catalog tag for the a < b path after sign
 * normalize; does not call libc memcmp. No parent wires.
 */
int
gj_memcmp_order_lt_id_8062(void)
{
	(void)NULL;
	return b8062_lt_id();
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_memcmp_order_lt_id_8062(void)
    __attribute__((alias("gj_memcmp_order_lt_id_8062")));
