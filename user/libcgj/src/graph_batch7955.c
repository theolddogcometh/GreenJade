/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7955: qsort key-equal predicate stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_qsort_cmp_eq_u_7955(uint32_t a, uint32_t b);
 *     - Return 1 if a == b, else 0.
 *   uint32_t __gj_qsort_cmp_eq_u_7955  (alias)
 *   __libcgj_batch7955_marker = "libcgj-batch7955"
 *
 * Exclusive continuum CREATE-ONLY (7951-7960: qsort compare stubs —
 * u32_asc, u32_desc, i32_asc, i32_desc, eq_u, lt_u, sign_norm,
 * result_ok, stable_idx, continuum + batch_id_7960). Unique
 * gj_qsort_cmp_eq_u_7955 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7955_marker[] = "libcgj-batch7955";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7955_eq(uint32_t u32A, uint32_t u32B)
{
	if (u32A == u32B)
		return 1u;
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_qsort_cmp_eq_u_7955 - 1 if two qsort keys are equal.
 *
 * a: left key
 * b: right key
 *
 * Returns 1 when a == b, else 0. Soft equality gate for tie-break /
 * stable paths; does not call libc. No parent wires.
 */
uint32_t
gj_qsort_cmp_eq_u_7955(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b7955_eq(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_qsort_cmp_eq_u_7955(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_qsort_cmp_eq_u_7955")));
