/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7757: readdir cookie validity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_readdir_cookie_ok_u_7757(uint32_t cookie);
 *     - Return 1 if cookie is a known soft cookie code
 *       (start|mid|eof), else 0.
 *   uint32_t __gj_readdir_cookie_ok_u_7757  (alias)
 *   __libcgj_batch7757_marker = "libcgj-batch7757"
 *
 * Exclusive continuum CREATE-ONLY (7751-7760: readdir cookie stubs —
 * start_id, eof_id, mid_id, is_start, is_eof, is_mid, cookie_ok,
 * cookie_pack, cookie_errorish, continuum + batch_id_7760). Unique
 * gj_readdir_cookie_ok_u_7757 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7757_marker[] = "libcgj-batch7757";

/* Soft known cookie codes: start=0, mid=1, eof=0xffffffff. */
#define B7757_COOKIE_START  ((uint32_t)0u)
#define B7757_COOKIE_MID    ((uint32_t)1u)
#define B7757_COOKIE_EOF    ((uint32_t)0xffffffffu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7757_cookie_ok(uint32_t u32Cookie)
{
	if (u32Cookie == B7757_COOKIE_START) {
		return 1u;
	}
	if (u32Cookie == B7757_COOKIE_MID) {
		return 1u;
	}
	if (u32Cookie == B7757_COOKIE_EOF) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_readdir_cookie_ok_u_7757 - 1 if cookie is a known soft cookie.
 *
 * cookie: soft readdir/telldir position cookie
 *
 * Returns 1 when cookie is 0, 1, or 0xffffffff; else 0. Soft catalog
 * check; does not call readdir. No parent wires.
 */
uint32_t
gj_readdir_cookie_ok_u_7757(uint32_t u32Cookie)
{
	(void)NULL;
	return b7757_cookie_ok(u32Cookie);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_readdir_cookie_ok_u_7757(uint32_t u32Cookie)
    __attribute__((alias("gj_readdir_cookie_ok_u_7757")));
