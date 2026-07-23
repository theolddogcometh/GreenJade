/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7779: scandir filter kind errorish/unknown stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_scandir_filter_errorish_u_7779(uint32_t kind);
 *     - Return 1 if kind is not a known soft filter kind
 *       (not accept-all/reject-dot/reject-hidden), else 0.
 *   uint32_t __gj_scandir_filter_errorish_u_7779  (alias)
 *   __libcgj_batch7779_marker = "libcgj-batch7779"
 *
 * Exclusive continuum CREATE-ONLY (7771-7780: scandir filter stubs —
 * accept_all_id, reject_dot_id, reject_hidden_id, is_accept_all,
 * is_reject_dot, is_reject_hidden, filter_ok, filter_pack,
 * filter_errorish, continuum + batch_id_7780). Unique
 * gj_scandir_filter_errorish_u_7779 surface only; no multi-def. No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7779_marker[] = "libcgj-batch7779";

/* Soft known filter kinds: accept-all=0, reject-dot=1, reject-hidden=2. */
#define B7779_FILTER_ACCEPT_ALL     ((uint32_t)0u)
#define B7779_FILTER_REJECT_DOT     ((uint32_t)1u)
#define B7779_FILTER_REJECT_HIDDEN  ((uint32_t)2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7779_filter_errorish(uint32_t u32Kind)
{
	if (u32Kind == B7779_FILTER_ACCEPT_ALL) {
		return 0u;
	}
	if (u32Kind == B7779_FILTER_REJECT_DOT) {
		return 0u;
	}
	if (u32Kind == B7779_FILTER_REJECT_HIDDEN) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_scandir_filter_errorish_u_7779 - 1 if kind is an unknown filter kind.
 *
 * kind: soft scandir filter kind code
 *
 * Returns 1 when kind is not 0, 1, or 2; else 0. Soft inverse catalog
 * check; does not call scandir. No parent wires.
 */
uint32_t
gj_scandir_filter_errorish_u_7779(uint32_t u32Kind)
{
	(void)NULL;
	return b7779_filter_errorish(u32Kind);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_scandir_filter_errorish_u_7779(uint32_t u32Kind)
    __attribute__((alias("gj_scandir_filter_errorish_u_7779")));
