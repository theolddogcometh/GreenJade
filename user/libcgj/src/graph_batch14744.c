/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14744: continuum crown peg identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14744(void);
 *     - Returns the compile-time graph batch number for this TU (14744).
 *   uint32_t gj_graph_milestone_14744(void);
 *     - Returns the current graph milestone revision (14744).
 *   uint32_t __gj_batch_id_14744  (alias)
 *   uint32_t __gj_graph_milestone_14744  (alias)
 *   __libcgj_batch14744_marker = "libcgj-batch14744"
 *
 * Exclusive continuum CREATE-ONLY wave (14726-14750). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14744_marker[] = "libcgj-batch14744";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14744_peg(void)
{
	return 14744u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14744 - report this TU's graph batch number.
 *
 * Always returns 14744.
 */
uint32_t
gj_batch_id_14744(void)
{
	(void)NULL;
	return b14744_peg();
}

/*
 * gj_graph_milestone_14744 - report this TU's graph milestone revision.
 *
 * Always returns 14744. Distinct from unsuffixed gj_graph_milestone so
 * all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_14744(void)
{
	return b14744_peg();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14744(void)
    __attribute__((alias("gj_batch_id_14744")));

uint32_t __gj_graph_milestone_14744(void)
    __attribute__((alias("gj_graph_milestone_14744")));
