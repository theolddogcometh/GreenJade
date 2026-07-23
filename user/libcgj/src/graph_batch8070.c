/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8070: memcmp order continuum lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_memcmp_order_continuum_ok_8070(void);
 *     - Returns 1 (soft lamp: 8061-8070 memcmp order stubs
 *       continuum complete / ready).
 *   uint32_t gj_batch_id_8070(void);
 *     - Returns the compile-time graph batch number for this TU (8070).
 *   uint32_t __gj_memcmp_order_continuum_ok_8070  (alias)
 *   uint32_t __gj_batch_id_8070  (alias)
 *   __libcgj_batch8070_marker = "libcgj-batch8070"
 *
 * Exclusive continuum CREATE-ONLY (8061-8070: memcmp order stubs —
 * eq_id, lt_id, gt_id, is_eq, is_lt, is_gt, is_ne, sign_norm,
 * u8_asc, continuum + batch_id_8070). Unique surfaces only; no
 * multi-def. Does NOT redefine bare gj_batch_id. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8070_marker[] = "libcgj-batch8070";

#define B8070_CONTINUUM_OK  1u
#define B8070_BATCH_ID      8070u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8070_continuum(void)
{
	return B8070_CONTINUUM_OK;
}

static uint32_t
b8070_id(void)
{
	return B8070_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_memcmp_order_continuum_ok_8070 - continuum-ready tag for 8061-8070.
 *
 * Always returns 1. Soft pure-data product tag that the memcmp order
 * stubs exclusive wave is present. Does not call libc.
 */
uint32_t
gj_memcmp_order_continuum_ok_8070(void)
{
	(void)NULL;
	return b8070_continuum();
}

/*
 * gj_batch_id_8070 - report this TU's graph batch number.
 *
 * Always returns 8070.
 */
uint32_t
gj_batch_id_8070(void)
{
	(void)NULL;
	return b8070_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_memcmp_order_continuum_ok_8070(void)
    __attribute__((alias("gj_memcmp_order_continuum_ok_8070")));

uint32_t __gj_batch_id_8070(void)
    __attribute__((alias("gj_batch_id_8070")));
