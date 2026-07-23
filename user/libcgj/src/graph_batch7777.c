/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7777: scandir filter kind validity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_scandir_filter_ok_u_7777(uint32_t kind);
 *     - Return 1 if kind is a known soft filter kind
 *       (accept-all|reject-dot|reject-hidden), else 0.
 *   uint32_t __gj_scandir_filter_ok_u_7777  (alias)
 *   __libcgj_batch7777_marker = "libcgj-batch7777"
 *
 * Exclusive continuum CREATE-ONLY (7771-7780: scandir filter stubs —
 * accept_all_id, reject_dot_id, reject_hidden_id, is_accept_all,
 * is_reject_dot, is_reject_hidden, filter_ok, filter_pack,
 * filter_errorish, continuum + batch_id_7780). Unique
 * gj_scandir_filter_ok_u_7777 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7777_marker[] = "libcgj-batch7777";

/* Soft known filter kinds: accept-all=0, reject-dot=1, reject-hidden=2. */
#define B7777_FILTER_ACCEPT_ALL     ((uint32_t)0u)
#define B7777_FILTER_REJECT_DOT     ((uint32_t)1u)
#define B7777_FILTER_REJECT_HIDDEN  ((uint32_t)2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7777_filter_ok(uint32_t u32Kind)
{
	if (u32Kind == B7777_FILTER_ACCEPT_ALL) {
		return 1u;
	}
	if (u32Kind == B7777_FILTER_REJECT_DOT) {
		return 1u;
	}
	if (u32Kind == B7777_FILTER_REJECT_HIDDEN) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_scandir_filter_ok_u_7777 - 1 if kind is a known scandir filter kind.
 *
 * kind: soft scandir filter kind code
 *
 * Returns 1 when kind is 0, 1, or 2; else 0. Soft catalog check; does
 * not call scandir. No parent wires.
 */
uint32_t
gj_scandir_filter_ok_u_7777(uint32_t u32Kind)
{
	(void)NULL;
	return b7777_filter_ok(u32Kind);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_scandir_filter_ok_u_7777(uint32_t u32Kind)
    __attribute__((alias("gj_scandir_filter_ok_u_7777")));
