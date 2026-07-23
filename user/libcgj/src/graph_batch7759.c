/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7759: readdir cookie errorish/unknown stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_readdir_cookie_errorish_u_7759(uint32_t cookie);
 *     - Return 1 if cookie is not a known soft cookie code
 *       (not start/mid/eof), else 0.
 *   uint32_t __gj_readdir_cookie_errorish_u_7759  (alias)
 *   __libcgj_batch7759_marker = "libcgj-batch7759"
 *
 * Exclusive continuum CREATE-ONLY (7751-7760: readdir cookie stubs —
 * start_id, eof_id, mid_id, is_start, is_eof, is_mid, cookie_ok,
 * cookie_pack, cookie_errorish, continuum + batch_id_7760). Unique
 * gj_readdir_cookie_errorish_u_7759 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7759_marker[] = "libcgj-batch7759";

/* Soft known cookie codes: start=0, mid=1, eof=0xffffffff. */
#define B7759_COOKIE_START  ((uint32_t)0u)
#define B7759_COOKIE_MID    ((uint32_t)1u)
#define B7759_COOKIE_EOF    ((uint32_t)0xffffffffu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7759_cookie_errorish(uint32_t u32Cookie)
{
	if (u32Cookie == B7759_COOKIE_START) {
		return 0u;
	}
	if (u32Cookie == B7759_COOKIE_MID) {
		return 0u;
	}
	if (u32Cookie == B7759_COOKIE_EOF) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_readdir_cookie_errorish_u_7759 - 1 if cookie is unknown/errorish.
 *
 * cookie: soft readdir/telldir position cookie
 *
 * Returns 1 when cookie is not 0, 1, or 0xffffffff; else 0. Soft
 * inverse of cookie_ok; does not call readdir. No parent wires.
 */
uint32_t
gj_readdir_cookie_errorish_u_7759(uint32_t u32Cookie)
{
	(void)NULL;
	return b7759_cookie_errorish(u32Cookie);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_readdir_cookie_errorish_u_7759(uint32_t u32Cookie)
    __attribute__((alias("gj_readdir_cookie_errorish_u_7759")));
