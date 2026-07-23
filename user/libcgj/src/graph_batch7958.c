/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7958: qsort compare-result ok predicate stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_qsort_cmp_result_ok_u_7958(int32_t r);
 *     - Return 1 if r is -1, 0, or 1; else 0.
 *   uint32_t __gj_qsort_cmp_result_ok_u_7958  (alias)
 *   __libcgj_batch7958_marker = "libcgj-batch7958"
 *
 * Exclusive continuum CREATE-ONLY (7951-7960: qsort compare stubs —
 * u32_asc, u32_desc, i32_asc, i32_desc, eq_u, lt_u, sign_norm,
 * result_ok, stable_idx, continuum + batch_id_7960). Unique
 * gj_qsort_cmp_result_ok_u_7958 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7958_marker[] = "libcgj-batch7958";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7958_result_ok(int32_t i32R)
{
	if (i32R == -1)
		return 1u;
	if (i32R == 0)
		return 1u;
	if (i32R == 1)
		return 1u;
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_qsort_cmp_result_ok_u_7958 - 1 if compare result is trichotomy-safe.
 *
 * r: raw comparator return value
 *
 * Returns 1 when r is exactly -1, 0, or 1; else 0. Soft validation
 * for normalized qsort comparators; does not call libc. No parent
 * wires.
 */
uint32_t
gj_qsort_cmp_result_ok_u_7958(int32_t i32R)
{
	(void)NULL;
	return b7958_result_ok(i32R);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_qsort_cmp_result_ok_u_7958(int32_t i32R)
    __attribute__((alias("gj_qsort_cmp_result_ok_u_7958")));
