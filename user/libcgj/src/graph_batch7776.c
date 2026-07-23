/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7776: scandir filter is-reject-hidden stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_scandir_filter_is_reject_hidden_u_7776(uint32_t kind);
 *     - Return 1 if kind == reject-hidden (2), else 0.
 *   uint32_t __gj_scandir_filter_is_reject_hidden_u_7776  (alias)
 *   __libcgj_batch7776_marker = "libcgj-batch7776"
 *
 * Exclusive continuum CREATE-ONLY (7771-7780: scandir filter stubs —
 * accept_all_id, reject_dot_id, reject_hidden_id, is_accept_all,
 * is_reject_dot, is_reject_hidden, filter_ok, filter_pack,
 * filter_errorish, continuum + batch_id_7780). Unique
 * gj_scandir_filter_is_reject_hidden_u_7776 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7776_marker[] = "libcgj-batch7776";

/* Soft reject-hidden filter kind code. */
#define B7776_FILTER_REJECT_HIDDEN  ((uint32_t)2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7776_is_reject_hidden(uint32_t u32Kind)
{
	return (u32Kind == B7776_FILTER_REJECT_HIDDEN) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_scandir_filter_is_reject_hidden_u_7776 - 1 if kind is reject-hidden.
 *
 * kind: soft scandir filter kind code
 *
 * Returns 1 when kind equals 2 (reject-hidden), else 0. Pure compare;
 * does not call scandir. No parent wires.
 */
uint32_t
gj_scandir_filter_is_reject_hidden_u_7776(uint32_t u32Kind)
{
	(void)NULL;
	return b7776_is_reject_hidden(u32Kind);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_scandir_filter_is_reject_hidden_u_7776(uint32_t u32Kind)
    __attribute__((alias("gj_scandir_filter_is_reject_hidden_u_7776")));
