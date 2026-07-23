/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7730: chdir path length continuum lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_chdir_path_len_continuum_ok_7730(void);
 *     - Returns 1 (soft lamp: 7721-7730 chdir path length stubs
 *       continuum complete / ready).
 *   uint32_t gj_batch_id_7730(void);
 *     - Returns the compile-time graph batch number for this TU (7730).
 *   uint32_t __gj_chdir_path_len_continuum_ok_7730  (alias)
 *   uint32_t __gj_batch_id_7730  (alias)
 *   __libcgj_batch7730_marker = "libcgj-batch7730"
 *
 * Exclusive continuum CREATE-ONLY (7721-7730: chdir path length stubs —
 * path_max, path_len_ok, nonempty, name_max, comp_len_ok, is_abs,
 * is_rel, path_room, req_ok, continuum + batch_id_7730). Unique surfaces
 * only; no multi-def. Does NOT redefine bare gj_batch_id. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7730_marker[] = "libcgj-batch7730";

#define B7730_CONTINUUM_OK  1u
#define B7730_BATCH_ID      7730u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7730_continuum(void)
{
	return B7730_CONTINUUM_OK;
}

static uint32_t
b7730_id(void)
{
	return B7730_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_chdir_path_len_continuum_ok_7730 - continuum-ready tag for 7721-7730.
 *
 * Always returns 1. Soft pure-data product tag that the chdir path
 * length stubs exclusive wave is present. Does not call libc.
 */
uint32_t
gj_chdir_path_len_continuum_ok_7730(void)
{
	(void)NULL;
	return b7730_continuum();
}

/*
 * gj_batch_id_7730 - report this TU's graph batch number.
 *
 * Always returns 7730.
 */
uint32_t
gj_batch_id_7730(void)
{
	return b7730_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_chdir_path_len_continuum_ok_7730(void)
    __attribute__((alias("gj_chdir_path_len_continuum_ok_7730")));

uint32_t __gj_batch_id_7730(void)
    __attribute__((alias("gj_batch_id_7730")));
