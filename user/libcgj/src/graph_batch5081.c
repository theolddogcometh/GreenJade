/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5081: debug constant true predicate (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dbg_true_u(void);
 *     - Always returns 1. Freestanding debug/assert continuum constant
 *       for "true" without relying on libc <stdbool.h> or macros.
 *   uint32_t __gj_dbg_true_u  (alias)
 *   __libcgj_batch5081_marker = "libcgj-batch5081"
 *
 * Exclusive continuum CREATE-ONLY (5081-5090: debug/assert unique —
 * dbg_true_u, dbg_false_u, dbg_likely_u, dbg_unlikely_u, dbg_assert_ok_u,
 * dbg_range_ok_u, dbg_ptr_ok_u, dbg_align_ok_u, dbg_not_zero_u,
 * batch_id_5090). Unique gj_dbg_true_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5081_marker[] = "libcgj-batch5081";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5081_true(void)
{
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dbg_true_u - constant true (1) for freestanding debug predicates.
 *
 * Always returns 1. Pair of gj_dbg_false_u (batch5082). No parent wires.
 */
uint32_t
gj_dbg_true_u(void)
{
	(void)NULL;
	return b5081_true();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dbg_true_u(void)
    __attribute__((alias("gj_dbg_true_u")));
