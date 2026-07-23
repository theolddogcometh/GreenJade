/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7620: getrlimit resource stubs continuum lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rlim_resource_continuum_ok_7620(void);
 *     - Returns 1 (soft lamp: 7611-7620 getrlimit resource stubs
 *       continuum complete / ready).
 *   uint32_t gj_batch_id_7620(void);
 *     - Returns the compile-time graph batch number for this TU (7620).
 *   uint32_t __gj_rlim_resource_continuum_ok_7620  (alias)
 *   uint32_t __gj_batch_id_7620  (alias)
 *   __libcgj_batch7620_marker = "libcgj-batch7620"
 *
 * Exclusive continuum CREATE-ONLY (7611-7620: getrlimit resource stubs —
 * infinity, nlimits, resource_ok, is_inf, soft_le_hard, cpu_id,
 * nofile_id, as_id, pair_ok, continuum + batch_id_7620). Unique surfaces
 * only; no multi-def. Does NOT redefine bare gj_batch_id. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7620_marker[] = "libcgj-batch7620";

#define B7620_CONTINUUM_OK  1u
#define B7620_BATCH_ID      7620u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7620_continuum(void)
{
	return B7620_CONTINUUM_OK;
}

static uint32_t
b7620_id(void)
{
	return B7620_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rlim_resource_continuum_ok_7620 - continuum-ready tag for 7611-7620.
 *
 * Always returns 1. Soft pure-data product tag that the getrlimit
 * resource stubs exclusive wave is present. Does not call libc.
 */
uint32_t
gj_rlim_resource_continuum_ok_7620(void)
{
	(void)NULL;
	return b7620_continuum();
}

/*
 * gj_batch_id_7620 - report this TU's graph batch number.
 *
 * Always returns 7620.
 */
uint32_t
gj_batch_id_7620(void)
{
	return b7620_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_rlim_resource_continuum_ok_7620(void)
    __attribute__((alias("gj_rlim_resource_continuum_ok_7620")));

uint32_t __gj_batch_id_7620(void)
    __attribute__((alias("gj_batch_id_7620")));
