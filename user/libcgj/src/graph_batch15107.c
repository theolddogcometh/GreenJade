/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch15107: continuum CREATE-ONLY batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_15107(void);
 *     - Returns the compile-time graph batch number for this TU (15107).
 *   uint32_t gj_graph_milestone_15107(void);
 *     - Returns the current graph milestone revision (15107).
 *   uint32_t __gj_batch_id_15107  (alias)
 *   uint32_t __gj_graph_milestone_15107  (alias)
 *   __libcgj_batch15107_marker = "libcgj-batch15107"
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

/* CGJ TU marker: greppable continuum identity string for batch 15107. */
const char __libcgj_batch15107_marker[] = "libcgj-batch15107";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b15107_id(void)
{
	return 15107u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_15107 - report this TU's graph batch number.
 *
 * Always returns 15107u. Soft pure-data batch identity lamp.
 * Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 15107.
 */
uint32_t
gj_batch_id_15107(void)
{
	(void)NULL;
	return b15107_id();
}

/*
 * gj_graph_milestone_15107 - report this TU's graph milestone revision.
 *
 * Always returns 15107u. Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 15107.
 */
uint32_t
gj_graph_milestone_15107(void)
{
	return b15107_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_15107(void)
    __attribute__((alias("gj_batch_id_15107")));

uint32_t __gj_graph_milestone_15107(void)
    __attribute__((alias("gj_graph_milestone_15107")));
