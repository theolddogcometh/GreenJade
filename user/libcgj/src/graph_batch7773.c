/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7773: scandir filter reject-hidden kind id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_scandir_filter_reject_hidden_id_7773(void);
 *     - Return soft reject-hidden filter kind code (2): drop names with
 *       a leading '.' (including "." / "..").
 *   uint32_t __gj_scandir_filter_reject_hidden_id_7773  (alias)
 *   __libcgj_batch7773_marker = "libcgj-batch7773"
 *
 * Exclusive continuum CREATE-ONLY (7771-7780: scandir filter stubs —
 * accept_all_id, reject_dot_id, reject_hidden_id, is_accept_all,
 * is_reject_dot, is_reject_hidden, filter_ok, filter_pack,
 * filter_errorish, continuum + batch_id_7780). Unique
 * gj_scandir_filter_reject_hidden_id_7773 surface only; no multi-def. No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7773_marker[] = "libcgj-batch7773";

/* Soft scandir filter kind: reject hidden (leading '.') names. */
#define B7773_FILTER_REJECT_HIDDEN  ((uint32_t)2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7773_reject_hidden_id(void)
{
	return B7773_FILTER_REJECT_HIDDEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_scandir_filter_reject_hidden_id_7773 - soft reject-hidden filter kind.
 *
 * Always returns 2. Catalog id only; does not call scandir. No parent
 * wires.
 */
uint32_t
gj_scandir_filter_reject_hidden_id_7773(void)
{
	(void)NULL;
	return b7773_reject_hidden_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_scandir_filter_reject_hidden_id_7773(void)
    __attribute__((alias("gj_scandir_filter_reject_hidden_id_7773")));
