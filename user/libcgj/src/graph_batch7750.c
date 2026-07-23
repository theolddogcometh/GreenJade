/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7750: getdents dirent continuum lamp +
 * exclusive batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_getdents_dirent_continuum_ok_7750(void);
 *     - Returns 1 (soft lamp: 7741-7750 getdents dirent stubs
 *       continuum complete / ready).
 *   uint32_t gj_batch_id_7750(void);
 *     - Returns the compile-time graph batch number for this TU (7750).
 *   uint32_t __gj_getdents_dirent_continuum_ok_7750  (alias)
 *   uint32_t __gj_batch_id_7750  (alias)
 *   __libcgj_batch7750_marker = "libcgj-batch7750"
 *
 * Exclusive continuum CREATE-ONLY (7741-7750: getdents dirent stubs —
 * root, getdents_nr, getdents64_nr, dt_dir, dt_reg, dt_ok, reclen_align,
 * reclen_min, dt_errorish, continuum + batch_id_7750). Unique surfaces
 * only; no multi-def. Does NOT redefine bare gj_batch_id.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7750_marker[] = "libcgj-batch7750";

#define B7750_CONTINUUM_OK  1u
#define B7750_BATCH_ID      7750u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7750_continuum(void)
{
	return B7750_CONTINUUM_OK;
}

static uint32_t
b7750_id(void)
{
	return B7750_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_getdents_dirent_continuum_ok_7750 - continuum-ready tag for 7741-7750.
 *
 * Always returns 1. Soft pure-data product tag that the getdents dirent
 * stubs exclusive wave is present. Does not call libc.
 */
uint32_t
gj_getdents_dirent_continuum_ok_7750(void)
{
	(void)NULL;
	return b7750_continuum();
}

/*
 * gj_batch_id_7750 - report this TU's graph batch number.
 *
 * Always returns 7750.
 */
uint32_t
gj_batch_id_7750(void)
{
	return b7750_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_getdents_dirent_continuum_ok_7750(void)
    __attribute__((alias("gj_getdents_dirent_continuum_ok_7750")));

uint32_t __gj_batch_id_7750(void)
    __attribute__((alias("gj_batch_id_7750")));
