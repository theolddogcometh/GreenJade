/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7760: readdir cookie continuum lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_readdir_cookie_continuum_ok_7760(void);
 *     - Returns 1 (soft lamp: 7751-7760 readdir cookie stubs
 *       continuum complete / ready).
 *   uint32_t gj_batch_id_7760(void);
 *     - Returns the compile-time graph batch number for this TU (7760).
 *   uint32_t __gj_readdir_cookie_continuum_ok_7760  (alias)
 *   uint32_t __gj_batch_id_7760  (alias)
 *   __libcgj_batch7760_marker = "libcgj-batch7760"
 *
 * Exclusive continuum CREATE-ONLY (7751-7760: readdir cookie stubs —
 * start_id, eof_id, mid_id, is_start, is_eof, is_mid, cookie_ok,
 * cookie_pack, cookie_errorish, continuum + batch_id_7760). Unique
 * surfaces only; no multi-def. Does NOT redefine bare gj_batch_id. No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7760_marker[] = "libcgj-batch7760";

#define B7760_CONTINUUM_OK  1u
#define B7760_BATCH_ID      7760u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7760_continuum(void)
{
	return B7760_CONTINUUM_OK;
}

static uint32_t
b7760_id(void)
{
	return B7760_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_readdir_cookie_continuum_ok_7760 - continuum-ready tag for 7751-7760.
 *
 * Always returns 1. Soft pure-data product tag that the readdir cookie
 * stubs exclusive wave is present. Does not call libc.
 */
uint32_t
gj_readdir_cookie_continuum_ok_7760(void)
{
	(void)NULL;
	return b7760_continuum();
}

/*
 * gj_batch_id_7760 - report this TU's graph batch number.
 *
 * Always returns 7760.
 */
uint32_t
gj_batch_id_7760(void)
{
	return b7760_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_readdir_cookie_continuum_ok_7760(void)
    __attribute__((alias("gj_readdir_cookie_continuum_ok_7760")));

uint32_t __gj_batch_id_7760(void)
    __attribute__((alias("gj_batch_id_7760")));
