/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7756: readdir cookie is-mid stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_readdir_is_mid_cookie_u_7756(uint32_t cookie);
 *     - Return 1 if cookie == mid catalog id (1), else 0.
 *   uint32_t __gj_readdir_is_mid_cookie_u_7756  (alias)
 *   __libcgj_batch7756_marker = "libcgj-batch7756"
 *
 * Exclusive continuum CREATE-ONLY (7751-7760: readdir cookie stubs —
 * start_id, eof_id, mid_id, is_start, is_eof, is_mid, cookie_ok,
 * cookie_pack, cookie_errorish, continuum + batch_id_7760). Unique
 * gj_readdir_is_mid_cookie_u_7756 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7756_marker[] = "libcgj-batch7756";

/* Soft readdir mid-stream cookie catalog id. */
#define B7756_COOKIE_MID  ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7756_is_mid(uint32_t u32Cookie)
{
	return (u32Cookie == B7756_COOKIE_MID) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_readdir_is_mid_cookie_u_7756 - 1 if cookie is mid-stream catalog id.
 *
 * cookie: soft readdir/telldir position cookie
 *
 * Returns 1 when cookie equals 1 (mid), else 0. Pure compare; does not
 * call readdir. No parent wires.
 */
uint32_t
gj_readdir_is_mid_cookie_u_7756(uint32_t u32Cookie)
{
	(void)NULL;
	return b7756_is_mid(u32Cookie);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_readdir_is_mid_cookie_u_7756(uint32_t u32Cookie)
    __attribute__((alias("gj_readdir_is_mid_cookie_u_7756")));
