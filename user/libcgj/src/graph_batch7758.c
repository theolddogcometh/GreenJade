/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7758: readdir cookie pack stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_readdir_cookie_pack_u_7758(uint32_t want_start,
 *                                          uint32_t want_mid,
 *                                          uint32_t want_eof);
 *     - Soft pack: non-zero want_start → bit0 (1), want_mid → bit1
 *       (2), want_eof → bit2 (4); OR selected presence bits.
 *   uint32_t __gj_readdir_cookie_pack_u_7758  (alias)
 *   __libcgj_batch7758_marker = "libcgj-batch7758"
 *
 * Exclusive continuum CREATE-ONLY (7751-7760: readdir cookie stubs —
 * start_id, eof_id, mid_id, is_start, is_eof, is_mid, cookie_ok,
 * cookie_pack, cookie_errorish, continuum + batch_id_7760). Unique
 * gj_readdir_cookie_pack_u_7758 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7758_marker[] = "libcgj-batch7758";

/* Soft presence bits for packed cookie-kind catalog (not the cookies). */
#define B7758_PACK_START  ((uint32_t)0x00000001u)
#define B7758_PACK_MID    ((uint32_t)0x00000002u)
#define B7758_PACK_EOF    ((uint32_t)0x00000004u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7758_cookie_pack(uint32_t u32WantStart, uint32_t u32WantMid,
    uint32_t u32WantEof)
{
	uint32_t u32Out = 0u;

	if (u32WantStart != 0u) {
		u32Out |= B7758_PACK_START;
	}
	if (u32WantMid != 0u) {
		u32Out |= B7758_PACK_MID;
	}
	if (u32WantEof != 0u) {
		u32Out |= B7758_PACK_EOF;
	}
	return u32Out;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_readdir_cookie_pack_u_7758 - soft pack of cookie-kind request bits.
 *
 * want_start: non-zero to include start presence bit
 * want_mid:   non-zero to include mid presence bit
 * want_eof:   non-zero to include EOF presence bit
 *
 * Returns packed presence mask (bits 0..2). Pure integer; does not call
 * readdir. No parent wires.
 */
uint32_t
gj_readdir_cookie_pack_u_7758(uint32_t want_start, uint32_t want_mid,
    uint32_t want_eof)
{
	(void)NULL;
	return b7758_cookie_pack(want_start, want_mid, want_eof);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_readdir_cookie_pack_u_7758(uint32_t want_start,
    uint32_t want_mid, uint32_t want_eof)
    __attribute__((alias("gj_readdir_cookie_pack_u_7758")));
