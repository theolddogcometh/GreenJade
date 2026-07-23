/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7670: times clock_t continuum lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_times_clock_t_continuum_ok_7670(void);
 *     - Returns 1 (soft lamp: 7661-7670 times clock_t stubs continuum
 *       complete / ready).
 *   uint32_t gj_batch_id_7670(void);
 *     - Returns the compile-time graph batch number for this TU (7670).
 *   uint32_t __gj_times_clock_t_continuum_ok_7670  (alias)
 *   uint32_t __gj_batch_id_7670  (alias)
 *   __libcgj_batch7670_marker = "libcgj-batch7670"
 *
 * Exclusive continuum CREATE-ONLY (7661-7670: times clock_t stubs —
 * clocks_per_sec, tms_utime_id, tms_stime_id, tms_cutime_id,
 * tms_cstime_id, is_utime, field_ok, field_pack, clock_t_errorish,
 * continuum + batch_id_7670). Unique surfaces only; no multi-def. Does
 * NOT redefine bare gj_batch_id. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7670_marker[] = "libcgj-batch7670";

#define B7670_CONTINUUM_OK  1u
#define B7670_BATCH_ID      7670u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7670_continuum(void)
{
	return B7670_CONTINUUM_OK;
}

static uint32_t
b7670_id(void)
{
	return B7670_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_times_clock_t_continuum_ok_7670 - continuum-ready tag for 7661-7670.
 *
 * Always returns 1. Soft pure-data product tag that the times clock_t
 * stubs exclusive wave is present. Does not call libc.
 */
uint32_t
gj_times_clock_t_continuum_ok_7670(void)
{
	(void)NULL;
	return b7670_continuum();
}

/*
 * gj_batch_id_7670 - report this TU's graph batch number.
 *
 * Always returns 7670.
 */
uint32_t
gj_batch_id_7670(void)
{
	return b7670_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_times_clock_t_continuum_ok_7670(void)
    __attribute__((alias("gj_times_clock_t_continuum_ok_7670")));

uint32_t __gj_batch_id_7670(void)
    __attribute__((alias("gj_batch_id_7670")));
