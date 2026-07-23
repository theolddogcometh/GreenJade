/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5090: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5090(void);
 *     - Returns the compile-time graph batch number for this TU (5090).
 *   uint32_t __gj_batch_id_5090  (alias)
 *   __libcgj_batch5090_marker = "libcgj-batch5090"
 *
 * Exclusive continuum CREATE-ONLY (5081-5090: debug/assert unique —
 * dbg_true_u, dbg_false_u, dbg_likely_u, dbg_unlikely_u, dbg_assert_ok_u,
 * dbg_range_ok_u, dbg_ptr_ok_u, dbg_align_ok_u, dbg_not_zero_u,
 * batch_id_5090). Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5090_marker[] = "libcgj-batch5090";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5090_id(void)
{
	return 5090u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_5090 - report this TU's graph batch number.
 *
 * Always returns 5090. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_5090(void)
{
	(void)NULL;
	return b5090_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_5090(void)
    __attribute__((alias("gj_batch_id_5090")));
