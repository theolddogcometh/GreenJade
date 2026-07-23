/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7953: qsort i32 ascending compare stub.
 *
 * Surface (unique symbols):
 *   int gj_qsort_cmp_i32_asc_7953(int32_t a, int32_t b);
 *     - Return -1 if a < b, 0 if equal, 1 if a > b (signed asc).
 *   int __gj_qsort_cmp_i32_asc_7953  (alias)
 *   __libcgj_batch7953_marker = "libcgj-batch7953"
 *
 * Exclusive continuum CREATE-ONLY (7951-7960: qsort compare stubs —
 * u32_asc, u32_desc, i32_asc, i32_desc, eq_u, lt_u, sign_norm,
 * result_ok, stable_idx, continuum + batch_id_7960). Unique
 * gj_qsort_cmp_i32_asc_7953 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7953_marker[] = "libcgj-batch7953";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b7953_i32_asc(int32_t i32A, int32_t i32B)
{
	if (i32A < i32B)
		return -1;
	if (i32A > i32B)
		return 1;
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_qsort_cmp_i32_asc_7953 - signed 32-bit ascending qsort compare.
 *
 * a: left element value
 * b: right element value
 *
 * Returns -1 / 0 / 1. Branching only (no a-b overflow). Soft pure
 * compare stub; does not call libc qsort. No parent wires.
 */
int
gj_qsort_cmp_i32_asc_7953(int32_t i32A, int32_t i32B)
{
	(void)NULL;
	return b7953_i32_asc(i32A, i32B);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_qsort_cmp_i32_asc_7953(int32_t i32A, int32_t i32B)
    __attribute__((alias("gj_qsort_cmp_i32_asc_7953")));
