/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7751: readdir cookie start id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_readdir_cookie_start_id_7751(void);
 *     - Return soft readdir stream-start cookie (0): rewind/tell origin.
 *   uint32_t __gj_readdir_cookie_start_id_7751  (alias)
 *   __libcgj_batch7751_marker = "libcgj-batch7751"
 *
 * Exclusive continuum CREATE-ONLY (7751-7760: readdir cookie stubs —
 * start_id, eof_id, mid_id, is_start, is_eof, is_mid, cookie_ok,
 * cookie_pack, cookie_errorish, continuum + batch_id_7760). Unique
 * gj_readdir_cookie_start_id_7751 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7751_marker[] = "libcgj-batch7751";

/* Soft readdir start cookie: stream position origin (cookie = 0). */
#define B7751_COOKIE_START  ((uint32_t)0u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7751_cookie_start_id(void)
{
	return B7751_COOKIE_START;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_readdir_cookie_start_id_7751 - soft readdir start cookie constant.
 *
 * Always returns 0. Catalog id only; does not call readdir/telldir.
 * No parent wires.
 */
uint32_t
gj_readdir_cookie_start_id_7751(void)
{
	(void)NULL;
	return b7751_cookie_start_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_readdir_cookie_start_id_7751(void)
    __attribute__((alias("gj_readdir_cookie_start_id_7751")));
