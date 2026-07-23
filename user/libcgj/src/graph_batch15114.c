/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch15114: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_15114(void);
 *     - Returns the compile-time graph batch number for this TU (15114).
 *   uint32_t gj_graph_milestone_15114(void);
 *     - Returns the current graph milestone revision (15114).
 *   uint32_t __gj_batch_id_15114  (alias)
 *   uint32_t __gj_graph_milestone_15114  (alias)
 *   __libcgj_batch15114_marker = "libcgj-batch15114"
 *
 * GreenJade continuum CREATE-ONLY wave (batches 15101-15125 post-M=15100
 * continuum fill). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 15114. */
const char __libcgj_batch15114_marker[] = "libcgj-batch15114";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b15114_id(void)
{
	return 15114u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_15114 - report this TU's graph batch number.
 *
 * Always returns 15114u. Soft pure-data batch identity lamp.
 * Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 15114.
 */
uint32_t
gj_batch_id_15114(void)
{
	(void)NULL;
	return b15114_id();
}

/*
 * gj_graph_milestone_15114 - report this TU's graph milestone revision.
 *
 * Always returns 15114u. Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 15114.
 */
uint32_t
gj_graph_milestone_15114(void)
{
	return b15114_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_15114(void)
    __attribute__((alias("gj_batch_id_15114")));

uint32_t __gj_graph_milestone_15114(void)
    __attribute__((alias("gj_graph_milestone_15114")));
