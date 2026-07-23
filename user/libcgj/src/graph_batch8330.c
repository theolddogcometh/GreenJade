/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8330: qsort/bsearch compare stubs batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8330(void);
 *     - Returns the compile-time graph batch number for this TU (8330).
 *   uint32_t __gj_batch_id_8330  (alias)
 *   __libcgj_batch8330_marker = "libcgj-batch8330"
 *
 * Exclusive continuum CREATE-ONLY wave closer (8321-8330: qsort/bsearch
 * compare stubs — cmp_u32_lt, cmp_u32_eq, cmp_u32_gt, cmp_i32_sign,
 * bsearch_empty, bsearch_one, qsort_need, qsort_trivial, cmp_bytes,
 * batch_id_8330). Distinct from gj_batch_id / gj_batch_id_N of other
 * TUs — unique _8330 surface; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8330_marker[] = "libcgj-batch8330";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8330_id(void)
{
	return 8330u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8330 - report this TU's graph batch number.
 *
 * Always returns 8330.
 */
uint32_t
gj_batch_id_8330(void)
{
	(void)NULL;
	return b8330_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8330(void)
    __attribute__((alias("gj_batch_id_8330")));
