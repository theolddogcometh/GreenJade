/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7772: scandir filter reject-dot kind id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_scandir_filter_reject_dot_id_7772(void);
 *     - Return soft reject-dot filter kind code (1): drop "." and "..".
 *   uint32_t __gj_scandir_filter_reject_dot_id_7772  (alias)
 *   __libcgj_batch7772_marker = "libcgj-batch7772"
 *
 * Exclusive continuum CREATE-ONLY (7771-7780: scandir filter stubs —
 * accept_all_id, reject_dot_id, reject_hidden_id, is_accept_all,
 * is_reject_dot, is_reject_hidden, filter_ok, filter_pack,
 * filter_errorish, continuum + batch_id_7780). Unique
 * gj_scandir_filter_reject_dot_id_7772 surface only; no multi-def. No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7772_marker[] = "libcgj-batch7772";

/* Soft scandir filter kind: reject "." and ".." entries. */
#define B7772_FILTER_REJECT_DOT  ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7772_reject_dot_id(void)
{
	return B7772_FILTER_REJECT_DOT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_scandir_filter_reject_dot_id_7772 - soft reject-dot filter kind.
 *
 * Always returns 1. Catalog id only; does not call scandir. No parent
 * wires.
 */
uint32_t
gj_scandir_filter_reject_dot_id_7772(void)
{
	(void)NULL;
	return b7772_reject_dot_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_scandir_filter_reject_dot_id_7772(void)
    __attribute__((alias("gj_scandir_filter_reject_dot_id_7772")));
