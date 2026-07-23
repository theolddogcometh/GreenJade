/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7660: uname field length stubs continuum lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_uname_field_len_continuum_ok_7660(void);
 *     - Returns 1 (soft lamp: 7651-7660 uname field length stubs
 *       continuum complete / ready).
 *   uint32_t gj_batch_id_7660(void);
 *     - Returns the compile-time graph batch number for this TU (7660).
 *   uint32_t __gj_uname_field_len_continuum_ok_7660  (alias)
 *   uint32_t __gj_batch_id_7660  (alias)
 *   __libcgj_batch7660_marker = "libcgj-batch7660"
 *
 * Exclusive continuum CREATE-ONLY (7651-7660: uname field length stubs —
 * sysname_len, nodename_len, release_len, version_len, machine_len,
 * domainname_len, nmln, field_maxlen, field_len_ok, continuum +
 * batch_id_7660). Unique surfaces only; no multi-def. Does NOT redefine
 * bare gj_batch_id. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7660_marker[] = "libcgj-batch7660";

#define B7660_CONTINUUM_OK  1u
#define B7660_BATCH_ID      7660u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7660_continuum(void)
{
	return B7660_CONTINUUM_OK;
}

static uint32_t
b7660_id(void)
{
	return B7660_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uname_field_len_continuum_ok_7660 - continuum-ready tag for 7651-7660.
 *
 * Always returns 1. Soft pure-data product tag that the uname field
 * length stubs exclusive wave is present. Does not call libc.
 */
uint32_t
gj_uname_field_len_continuum_ok_7660(void)
{
	(void)NULL;
	return b7660_continuum();
}

/*
 * gj_batch_id_7660 - report this TU's graph batch number.
 *
 * Always returns 7660.
 */
uint32_t
gj_batch_id_7660(void)
{
	return b7660_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_uname_field_len_continuum_ok_7660(void)
    __attribute__((alias("gj_uname_field_len_continuum_ok_7660")));

uint32_t __gj_batch_id_7660(void)
    __attribute__((alias("gj_batch_id_7660")));
