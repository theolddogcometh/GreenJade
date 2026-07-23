/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8081: strcmp order equal constant stub.
 *
 * Surface (unique symbols):
 *   int32_t gj_strcmp_ord_eq_8081(void);
 *     - Return soft strcmp equal-order constant (0).
 *   int32_t __gj_strcmp_ord_eq_8081  (alias)
 *   __libcgj_batch8081_marker = "libcgj-batch8081"
 *
 * Exclusive continuum CREATE-ONLY (8081-8090: strcmp order stubs —
 * ord_eq, ord_lt, ord_gt, is_eq, is_lt, is_gt, is_le, is_ge, is_ne,
 * batch_id_8090). Unique gj_strcmp_ord_eq_8081 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8081_marker[] = "libcgj-batch8081";

/* Soft strcmp ternary order: equal. */
#define B8081_ORD_EQ  ((int32_t)0)

/* ---- freestanding helpers ---------------------------------------------- */

static int32_t
b8081_ord_eq(void)
{
	return B8081_ORD_EQ;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strcmp_ord_eq_8081 - soft strcmp equal-order constant.
 *
 * Always returns 0 (strings equal under strcmp order). Catalog
 * constant only; does not compare strings. No parent wires.
 */
int32_t
gj_strcmp_ord_eq_8081(void)
{
	(void)NULL;
	return b8081_ord_eq();
}

/* ---- underscored alias ------------------------------------------------- */

int32_t __gj_strcmp_ord_eq_8081(void)
    __attribute__((alias("gj_strcmp_ord_eq_8081")));
