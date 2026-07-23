/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7940: strtol base continuum lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strtol_base_continuum_ok_7940(void);
 *     - Returns 1 (soft lamp: 7931-7940 strtol base stubs continuum
 *       complete / ready).
 *   uint32_t gj_batch_id_7940(void);
 *     - Returns the compile-time graph batch number for this TU (7940).
 *   uint32_t __gj_strtol_base_continuum_ok_7940  (alias)
 *   uint32_t __gj_batch_id_7940  (alias)
 *   __libcgj_batch7940_marker = "libcgj-batch7940"
 *
 * Exclusive continuum CREATE-ONLY (7931-7940: strtol base stubs —
 * auto_id, bin_id, oct_id, dec_id, hex_id, min_id, max_id, base_ok,
 * base_errorish, continuum + batch_id_7940). Unique surfaces only; no
 * multi-def. Does NOT redefine bare gj_batch_id. No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7940_marker[] = "libcgj-batch7940";

#define B7940_CONTINUUM_OK  1u
#define B7940_BATCH_ID      7940u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7940_continuum(void)
{
	return B7940_CONTINUUM_OK;
}

static uint32_t
b7940_id(void)
{
	return B7940_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strtol_base_continuum_ok_7940 - continuum-ready tag for 7931-7940.
 *
 * Always returns 1. Soft pure-data product tag that the strtol base
 * stubs exclusive wave is present. Does not call libc.
 */
uint32_t
gj_strtol_base_continuum_ok_7940(void)
{
	(void)NULL;
	return b7940_continuum();
}

/*
 * gj_batch_id_7940 - report this TU's graph batch number.
 *
 * Always returns 7940.
 */
uint32_t
gj_batch_id_7940(void)
{
	return b7940_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_strtol_base_continuum_ok_7940(void)
    __attribute__((alias("gj_strtol_base_continuum_ok_7940")));

uint32_t __gj_batch_id_7940(void)
    __attribute__((alias("gj_batch_id_7940")));
