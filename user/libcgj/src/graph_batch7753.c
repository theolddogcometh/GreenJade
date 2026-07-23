/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7753: readdir cookie mid-stream id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_readdir_cookie_mid_id_7753(void);
 *     - Return soft mid-stream readdir cookie catalog id (1).
 *   uint32_t __gj_readdir_cookie_mid_id_7753  (alias)
 *   __libcgj_batch7753_marker = "libcgj-batch7753"
 *
 * Exclusive continuum CREATE-ONLY (7751-7760: readdir cookie stubs —
 * start_id, eof_id, mid_id, is_start, is_eof, is_mid, cookie_ok,
 * cookie_pack, cookie_errorish, continuum + batch_id_7760). Unique
 * gj_readdir_cookie_mid_id_7753 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7753_marker[] = "libcgj-batch7753";

/* Soft readdir mid-stream cookie catalog id (first non-start position). */
#define B7753_COOKIE_MID  ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7753_cookie_mid_id(void)
{
	return B7753_COOKIE_MID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_readdir_cookie_mid_id_7753 - soft readdir mid-stream cookie constant.
 *
 * Always returns 1. Catalog id only; does not call readdir/seekdir.
 * No parent wires.
 */
uint32_t
gj_readdir_cookie_mid_id_7753(void)
{
	(void)NULL;
	return b7753_cookie_mid_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_readdir_cookie_mid_id_7753(void)
    __attribute__((alias("gj_readdir_cookie_mid_id_7753")));
