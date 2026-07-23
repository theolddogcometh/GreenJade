/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14863: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14863(void);
 *     - Returns the compile-time graph batch number for this TU (14863).
 *   uint32_t gj_graph_milestone_14863(void);
 *     - Returns the current graph milestone revision (14863).
 *   uint32_t __gj_batch_id_14863  (alias)
 *   uint32_t __gj_graph_milestone_14863  (alias)
 *   __libcgj_batch14863_marker = "libcgj-batch14863"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 14851-14875). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14863_marker[] = "libcgj-batch14863";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14863_id(void)
{
	return 14863u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14863 - report this TU's graph batch number.
 *
 * Always returns 14863.
 */
uint32_t
gj_batch_id_14863(void)
{
	(void)NULL;
	return b14863_id();
}

/*
 * gj_graph_milestone_14863 - report this TU's graph milestone revision.
 *
 * Always returns 14863. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_14863(void)
{
	return b14863_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14863(void)
    __attribute__((alias("gj_batch_id_14863")));

uint32_t __gj_graph_milestone_14863(void)
    __attribute__((alias("gj_graph_milestone_14863")));
