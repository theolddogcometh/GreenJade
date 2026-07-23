/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7755: readdir cookie is-EOF stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_readdir_is_eof_cookie_u_7755(uint32_t cookie);
 *     - Return 1 if cookie == EOF (0xffffffff), else 0.
 *   uint32_t __gj_readdir_is_eof_cookie_u_7755  (alias)
 *   __libcgj_batch7755_marker = "libcgj-batch7755"
 *
 * Exclusive continuum CREATE-ONLY (7751-7760: readdir cookie stubs —
 * start_id, eof_id, mid_id, is_start, is_eof, is_mid, cookie_ok,
 * cookie_pack, cookie_errorish, continuum + batch_id_7760). Unique
 * gj_readdir_is_eof_cookie_u_7755 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7755_marker[] = "libcgj-batch7755";

/* Soft readdir EOF cookie. */
#define B7755_COOKIE_EOF  ((uint32_t)0xffffffffu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7755_is_eof(uint32_t u32Cookie)
{
	return (u32Cookie == B7755_COOKIE_EOF) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_readdir_is_eof_cookie_u_7755 - 1 if cookie is end-of-stream.
 *
 * cookie: soft readdir/telldir position cookie
 *
 * Returns 1 when cookie equals 0xffffffff (EOF), else 0. Pure compare;
 * does not call readdir. No parent wires.
 */
uint32_t
gj_readdir_is_eof_cookie_u_7755(uint32_t u32Cookie)
{
	(void)NULL;
	return b7755_is_eof(u32Cookie);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_readdir_is_eof_cookie_u_7755(uint32_t u32Cookie)
    __attribute__((alias("gj_readdir_is_eof_cookie_u_7755")));
