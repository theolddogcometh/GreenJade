/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7752: readdir cookie EOF id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_readdir_cookie_eof_id_7752(void);
 *     - Return soft readdir end-of-stream cookie (0xffffffff).
 *   uint32_t __gj_readdir_cookie_eof_id_7752  (alias)
 *   __libcgj_batch7752_marker = "libcgj-batch7752"
 *
 * Exclusive continuum CREATE-ONLY (7751-7760: readdir cookie stubs —
 * start_id, eof_id, mid_id, is_start, is_eof, is_mid, cookie_ok,
 * cookie_pack, cookie_errorish, continuum + batch_id_7760). Unique
 * gj_readdir_cookie_eof_id_7752 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7752_marker[] = "libcgj-batch7752";

/* Soft readdir EOF cookie: end-of-stream sentinel (all bits set). */
#define B7752_COOKIE_EOF  ((uint32_t)0xffffffffu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7752_cookie_eof_id(void)
{
	return B7752_COOKIE_EOF;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_readdir_cookie_eof_id_7752 - soft readdir EOF cookie constant.
 *
 * Always returns 0xffffffff. Catalog id only; does not call readdir.
 * No parent wires.
 */
uint32_t
gj_readdir_cookie_eof_id_7752(void)
{
	(void)NULL;
	return b7752_cookie_eof_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_readdir_cookie_eof_id_7752(void)
    __attribute__((alias("gj_readdir_cookie_eof_id_7752")));
