/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7960: qsort compare continuum lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_qsort_cmp_continuum_ok_7960(void);
 *     - Returns 1 (soft lamp: 7951-7960 qsort compare stubs
 *       continuum complete / ready).
 *   uint32_t gj_batch_id_7960(void);
 *     - Returns the compile-time graph batch number for this TU (7960).
 *   uint32_t __gj_qsort_cmp_continuum_ok_7960  (alias)
 *   uint32_t __gj_batch_id_7960  (alias)
 *   __libcgj_batch7960_marker = "libcgj-batch7960"
 *
 * Exclusive continuum CREATE-ONLY (7951-7960: qsort compare stubs —
 * u32_asc, u32_desc, i32_asc, i32_desc, eq_u, lt_u, sign_norm,
 * result_ok, stable_idx, continuum + batch_id_7960). Unique surfaces
 * only; no multi-def. Does NOT redefine bare gj_batch_id. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7960_marker[] = "libcgj-batch7960";

#define B7960_CONTINUUM_OK  1u
#define B7960_BATCH_ID      7960u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7960_continuum(void)
{
	return B7960_CONTINUUM_OK;
}

static uint32_t
b7960_id(void)
{
	return B7960_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_qsort_cmp_continuum_ok_7960 - continuum-ready tag for 7951-7960.
 *
 * Always returns 1. Soft pure-data product tag that the qsort compare
 * stubs exclusive wave is present. Does not call libc.
 */
uint32_t
gj_qsort_cmp_continuum_ok_7960(void)
{
	(void)NULL;
	return b7960_continuum();
}

/*
 * gj_batch_id_7960 - report this TU's graph batch number.
 *
 * Always returns 7960.
 */
uint32_t
gj_batch_id_7960(void)
{
	return b7960_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_qsort_cmp_continuum_ok_7960(void)
    __attribute__((alias("gj_qsort_cmp_continuum_ok_7960")));

uint32_t __gj_batch_id_7960(void)
    __attribute__((alias("gj_batch_id_7960")));
