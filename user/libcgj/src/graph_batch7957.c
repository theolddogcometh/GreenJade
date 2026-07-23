/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7957: qsort compare-result sign normalize stub.
 *
 * Surface (unique symbols):
 *   int gj_qsort_cmp_sign_norm_7957(int32_t delta);
 *     - Map any signed delta to -1 / 0 / 1.
 *   int __gj_qsort_cmp_sign_norm_7957  (alias)
 *   __libcgj_batch7957_marker = "libcgj-batch7957"
 *
 * Exclusive continuum CREATE-ONLY (7951-7960: qsort compare stubs —
 * u32_asc, u32_desc, i32_asc, i32_desc, eq_u, lt_u, sign_norm,
 * result_ok, stable_idx, continuum + batch_id_7960). Unique
 * gj_qsort_cmp_sign_norm_7957 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7957_marker[] = "libcgj-batch7957";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b7957_sign_norm(int32_t i32Delta)
{
	if (i32Delta < 0)
		return -1;
	if (i32Delta > 0)
		return 1;
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_qsort_cmp_sign_norm_7957 - normalize a raw compare delta to -1/0/1.
 *
 * delta: signed difference or raw comparator return
 *
 * Returns -1 if delta < 0, 0 if delta == 0, 1 if delta > 0. Soft
 * clamp for qsort-style trichotomy; does not call libc. No parent
 * wires.
 */
int
gj_qsort_cmp_sign_norm_7957(int32_t i32Delta)
{
	(void)NULL;
	return b7957_sign_norm(i32Delta);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_qsort_cmp_sign_norm_7957(int32_t i32Delta)
    __attribute__((alias("gj_qsort_cmp_sign_norm_7957")));
