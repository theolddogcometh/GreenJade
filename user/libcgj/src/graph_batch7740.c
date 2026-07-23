/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7740: fchdir fd ok continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fchdir_continuum_ok_7740(void);
 *     - Returns 1 (soft lamp: 7731-7740 fchdir fd ok stubs continuum
 *       complete / ready).
 *   uint32_t gj_batch_id_7740(void);
 *     - Returns the compile-time graph batch number for this TU (7740).
 *   uint32_t __gj_fchdir_continuum_ok_7740  (alias)
 *   uint32_t __gj_batch_id_7740  (alias)
 *   __libcgj_batch7740_marker = "libcgj-batch7740"
 *
 * Exclusive continuum CREATE-ONLY (7731-7740: fchdir fd ok stubs —
 * fd_ok, fd_errorish, fd_pos_ok, at_fdcwd_u, nr_x86_64, arg_count,
 * ebadf_num, enotdir_num, success_rc, continuum + batch_id_7740).
 * Unique surfaces only; no multi-def. Does NOT redefine bare
 * gj_batch_id. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7740_marker[] = "libcgj-batch7740";

#define B7740_CONTINUUM_OK  1u
#define B7740_BATCH_ID      7740u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7740_continuum(void)
{
	return B7740_CONTINUUM_OK;
}

static uint32_t
b7740_id(void)
{
	return B7740_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fchdir_continuum_ok_7740 - continuum-ready tag for 7731-7740.
 *
 * Always returns 1. Soft pure-data product tag that the fchdir fd ok
 * stubs exclusive wave is present. Does not call libc.
 */
uint32_t
gj_fchdir_continuum_ok_7740(void)
{
	(void)NULL;
	return b7740_continuum();
}

/*
 * gj_batch_id_7740 - report this TU's graph batch number.
 *
 * Always returns 7740.
 */
uint32_t
gj_batch_id_7740(void)
{
	return b7740_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_fchdir_continuum_ok_7740(void)
    __attribute__((alias("gj_fchdir_continuum_ok_7740")));

uint32_t __gj_batch_id_7740(void)
    __attribute__((alias("gj_batch_id_7740")));
