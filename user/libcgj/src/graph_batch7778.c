/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7778: scandir filter kind pack stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_scandir_filter_pack_u_7778(uint32_t want_accept_all,
 *                                          uint32_t want_reject_dot,
 *                                          uint32_t want_reject_hidden);
 *     - Soft pack: non-zero want_accept_all → bit0 (1), want_reject_dot
 *       → bit1 (2), want_reject_hidden → bit2 (4); OR selected presence
 *       bits.
 *   uint32_t __gj_scandir_filter_pack_u_7778  (alias)
 *   __libcgj_batch7778_marker = "libcgj-batch7778"
 *
 * Exclusive continuum CREATE-ONLY (7771-7780: scandir filter stubs —
 * accept_all_id, reject_dot_id, reject_hidden_id, is_accept_all,
 * is_reject_dot, is_reject_hidden, filter_ok, filter_pack,
 * filter_errorish, continuum + batch_id_7780). Unique
 * gj_scandir_filter_pack_u_7778 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7778_marker[] = "libcgj-batch7778";

/* Soft presence bits for packed filter-kind catalog (not the kind codes). */
#define B7778_PACK_ACCEPT_ALL     ((uint32_t)0x00000001u)
#define B7778_PACK_REJECT_DOT     ((uint32_t)0x00000002u)
#define B7778_PACK_REJECT_HIDDEN  ((uint32_t)0x00000004u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7778_filter_pack(uint32_t u32WantAcceptAll, uint32_t u32WantRejectDot,
    uint32_t u32WantRejectHidden)
{
	uint32_t u32Out = 0u;

	if (u32WantAcceptAll != 0u) {
		u32Out |= B7778_PACK_ACCEPT_ALL;
	}
	if (u32WantRejectDot != 0u) {
		u32Out |= B7778_PACK_REJECT_DOT;
	}
	if (u32WantRejectHidden != 0u) {
		u32Out |= B7778_PACK_REJECT_HIDDEN;
	}
	return u32Out;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_scandir_filter_pack_u_7778 - soft pack of filter-kind request bits.
 *
 * want_accept_all:    non-zero to include accept-all presence bit
 * want_reject_dot:    non-zero to include reject-dot presence bit
 * want_reject_hidden: non-zero to include reject-hidden presence bit
 *
 * Returns packed presence mask (bits 0..2). Pure integer; does not call
 * scandir. No parent wires.
 */
uint32_t
gj_scandir_filter_pack_u_7778(uint32_t want_accept_all,
    uint32_t want_reject_dot, uint32_t want_reject_hidden)
{
	(void)NULL;
	return b7778_filter_pack(want_accept_all, want_reject_dot,
	    want_reject_hidden);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_scandir_filter_pack_u_7778(uint32_t want_accept_all,
    uint32_t want_reject_dot, uint32_t want_reject_hidden)
    __attribute__((alias("gj_scandir_filter_pack_u_7778")));
