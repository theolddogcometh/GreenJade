/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5082: debug constant false predicate (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dbg_false_u(void);
 *     - Always returns 0. Freestanding debug/assert continuum constant
 *       for "false" without relying on libc <stdbool.h> or macros.
 *   uint32_t __gj_dbg_false_u  (alias)
 *   __libcgj_batch5082_marker = "libcgj-batch5082"
 *
 * Exclusive continuum CREATE-ONLY (5081-5090: debug/assert unique).
 * Unique gj_dbg_false_u surface only; no multi-def. Distinct from
 * gj_dbg_true_u (batch5081). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5082_marker[] = "libcgj-batch5082";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5082_false(void)
{
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dbg_false_u - constant false (0) for freestanding debug predicates.
 *
 * Always returns 0. Pair of gj_dbg_true_u (batch5081). No parent wires.
 */
uint32_t
gj_dbg_false_u(void)
{
	(void)NULL;
	return b5082_false();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dbg_false_u(void)
    __attribute__((alias("gj_dbg_false_u")));
