/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7774: scandir filter is-accept-all stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_scandir_filter_is_accept_all_u_7774(uint32_t kind);
 *     - Return 1 if kind == accept-all (0), else 0.
 *   uint32_t __gj_scandir_filter_is_accept_all_u_7774  (alias)
 *   __libcgj_batch7774_marker = "libcgj-batch7774"
 *
 * Exclusive continuum CREATE-ONLY (7771-7780: scandir filter stubs —
 * accept_all_id, reject_dot_id, reject_hidden_id, is_accept_all,
 * is_reject_dot, is_reject_hidden, filter_ok, filter_pack,
 * filter_errorish, continuum + batch_id_7780). Unique
 * gj_scandir_filter_is_accept_all_u_7774 surface only; no multi-def. No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7774_marker[] = "libcgj-batch7774";

/* Soft accept-all filter kind code. */
#define B7774_FILTER_ACCEPT_ALL  ((uint32_t)0u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7774_is_accept_all(uint32_t u32Kind)
{
	return (u32Kind == B7774_FILTER_ACCEPT_ALL) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_scandir_filter_is_accept_all_u_7774 - 1 if kind is accept-all.
 *
 * kind: soft scandir filter kind code
 *
 * Returns 1 when kind equals 0 (accept-all), else 0. Pure compare; does
 * not call scandir. No parent wires.
 */
uint32_t
gj_scandir_filter_is_accept_all_u_7774(uint32_t u32Kind)
{
	(void)NULL;
	return b7774_is_accept_all(u32Kind);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_scandir_filter_is_accept_all_u_7774(uint32_t u32Kind)
    __attribute__((alias("gj_scandir_filter_is_accept_all_u_7774")));
