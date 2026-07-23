/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8083: strcmp order greater-than constant stub.
 *
 * Surface (unique symbols):
 *   int32_t gj_strcmp_ord_gt_8083(void);
 *     - Return soft strcmp greater-than-order constant (1).
 *   int32_t __gj_strcmp_ord_gt_8083  (alias)
 *   __libcgj_batch8083_marker = "libcgj-batch8083"
 *
 * Exclusive continuum CREATE-ONLY (8081-8090: strcmp order stubs —
 * ord_eq, ord_lt, ord_gt, is_eq, is_lt, is_gt, is_le, is_ge, is_ne,
 * batch_id_8090). Unique gj_strcmp_ord_gt_8083 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8083_marker[] = "libcgj-batch8083";

/* Soft strcmp ternary order: greater-than. */
#define B8083_ORD_GT  ((int32_t)1)

/* ---- freestanding helpers ---------------------------------------------- */

static int32_t
b8083_ord_gt(void)
{
	return B8083_ORD_GT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strcmp_ord_gt_8083 - soft strcmp greater-than-order constant.
 *
 * Always returns 1 (a > b under strcmp order). Catalog constant only;
 * does not compare strings. No parent wires.
 */
int32_t
gj_strcmp_ord_gt_8083(void)
{
	(void)NULL;
	return b8083_ord_gt();
}

/* ---- underscored alias ------------------------------------------------- */

int32_t __gj_strcmp_ord_gt_8083(void)
    __attribute__((alias("gj_strcmp_ord_gt_8083")));
