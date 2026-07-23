/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14739: continuum vault hop identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14739(void);
 *     - Returns the compile-time graph batch number for this TU (14739).
 *   uint32_t gj_graph_milestone_14739(void);
 *     - Returns the current graph milestone revision (14739).
 *   uint32_t __gj_batch_id_14739  (alias)
 *   uint32_t __gj_graph_milestone_14739  (alias)
 *   __libcgj_batch14739_marker = "libcgj-batch14739"
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

const char __libcgj_batch14739_marker[] = "libcgj-batch14739";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14739_hop(void)
{
	return 14739u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14739 - report this TU's graph batch number.
 *
 * Always returns 14739.
 */
uint32_t
gj_batch_id_14739(void)
{
	(void)NULL;
	return b14739_hop();
}

/*
 * gj_graph_milestone_14739 - report this TU's graph milestone revision.
 *
 * Always returns 14739. Distinct from unsuffixed gj_graph_milestone so
 * all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_14739(void)
{
	return b14739_hop();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14739(void)
    __attribute__((alias("gj_batch_id_14739")));

uint32_t __gj_graph_milestone_14739(void)
    __attribute__((alias("gj_graph_milestone_14739")));
