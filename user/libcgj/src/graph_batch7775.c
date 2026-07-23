/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7775: scandir filter is-reject-dot stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_scandir_filter_is_reject_dot_u_7775(uint32_t kind);
 *     - Return 1 if kind == reject-dot (1), else 0.
 *   uint32_t __gj_scandir_filter_is_reject_dot_u_7775  (alias)
 *   __libcgj_batch7775_marker = "libcgj-batch7775"
 *
 * Exclusive continuum CREATE-ONLY (7771-7780: scandir filter stubs —
 * accept_all_id, reject_dot_id, reject_hidden_id, is_accept_all,
 * is_reject_dot, is_reject_hidden, filter_ok, filter_pack,
 * filter_errorish, continuum + batch_id_7780). Unique
 * gj_scandir_filter_is_reject_dot_u_7775 surface only; no multi-def. No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7775_marker[] = "libcgj-batch7775";

/* Soft reject-dot filter kind code. */
#define B7775_FILTER_REJECT_DOT  ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7775_is_reject_dot(uint32_t u32Kind)
{
	return (u32Kind == B7775_FILTER_REJECT_DOT) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_scandir_filter_is_reject_dot_u_7775 - 1 if kind is reject-dot.
 *
 * kind: soft scandir filter kind code
 *
 * Returns 1 when kind equals 1 (reject-dot), else 0. Pure compare; does
 * not call scandir. No parent wires.
 */
uint32_t
gj_scandir_filter_is_reject_dot_u_7775(uint32_t u32Kind)
{
	(void)NULL;
	return b7775_is_reject_dot(u32Kind);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_scandir_filter_is_reject_dot_u_7775(uint32_t u32Kind)
    __attribute__((alias("gj_scandir_filter_is_reject_dot_u_7775")));
