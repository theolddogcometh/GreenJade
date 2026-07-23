/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7771: scandir filter accept-all kind id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_scandir_filter_accept_all_id_7771(void);
 *     - Return soft accept-all filter kind code (0): keep every entry.
 *   uint32_t __gj_scandir_filter_accept_all_id_7771  (alias)
 *   __libcgj_batch7771_marker = "libcgj-batch7771"
 *
 * Exclusive continuum CREATE-ONLY (7771-7780: scandir filter stubs —
 * accept_all_id, reject_dot_id, reject_hidden_id, is_accept_all,
 * is_reject_dot, is_reject_hidden, filter_ok, filter_pack,
 * filter_errorish, continuum + batch_id_7780). Unique
 * gj_scandir_filter_accept_all_id_7771 surface only; no multi-def. No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7771_marker[] = "libcgj-batch7771";

/* Soft scandir filter kind: accept all entries (NULL-filter analog). */
#define B7771_FILTER_ACCEPT_ALL  ((uint32_t)0u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7771_accept_all_id(void)
{
	return B7771_FILTER_ACCEPT_ALL;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_scandir_filter_accept_all_id_7771 - soft accept-all filter kind.
 *
 * Always returns 0. Catalog id only; does not call scandir. No parent
 * wires.
 */
uint32_t
gj_scandir_filter_accept_all_id_7771(void)
{
	(void)NULL;
	return b7771_accept_all_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_scandir_filter_accept_all_id_7771(void)
    __attribute__((alias("gj_scandir_filter_accept_all_id_7771")));
