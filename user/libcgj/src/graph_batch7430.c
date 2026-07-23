/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7430: access mode R_OK W_OK continuum lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_access_modes_continuum_ok_7430(void);
 *     - Returns 1 (soft lamp: 7421-7430 access mode R_OK W_OK stubs
 *       continuum complete / ready).
 *   uint32_t gj_batch_id_7430(void);
 *     - Returns the compile-time graph batch number for this TU (7430).
 *   uint32_t __gj_access_modes_continuum_ok_7430  (alias)
 *   uint32_t __gj_batch_id_7430  (alias)
 *   __libcgj_batch7430_marker = "libcgj-batch7430"
 *
 * Exclusive continuum CREATE-ONLY (7421-7430: access mode R_OK W_OK stubs —
 * r_ok_u, w_ok_u, x_ok_u, rw_mask, known_modes, known_modes_ok, has_rw,
 * rw_both_u, any_u, continuum + batch_id_7430). Unique surfaces only;
 * no multi-def. Does NOT redefine bare gj_batch_id. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7430_marker[] = "libcgj-batch7430";

#define B7430_CONTINUUM_OK  1u
#define B7430_BATCH_ID      7430u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7430_continuum(void)
{
	return B7430_CONTINUUM_OK;
}

static uint32_t
b7430_id(void)
{
	return B7430_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_access_modes_continuum_ok_7430 - continuum-ready tag for 7421-7430.
 *
 * Always returns 1. Soft pure-data product tag that the access mode
 * R_OK W_OK stubs exclusive wave is present. Does not call libc.
 */
uint32_t
gj_access_modes_continuum_ok_7430(void)
{
	(void)NULL;
	return b7430_continuum();
}

/*
 * gj_batch_id_7430 - report this TU's graph batch number.
 *
 * Always returns 7430.
 */
uint32_t
gj_batch_id_7430(void)
{
	return b7430_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_access_modes_continuum_ok_7430(void)
    __attribute__((alias("gj_access_modes_continuum_ok_7430")));

uint32_t __gj_batch_id_7430(void)
    __attribute__((alias("gj_batch_id_7430")));
