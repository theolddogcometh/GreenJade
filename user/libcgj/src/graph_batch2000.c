/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2000: milestone 2000 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2000(void);
 *     — Returns the compile-time graph batch number for this TU (2000).
 *   uint32_t gj_graph_milestone_2000(void);
 *     — Returns the current graph milestone revision (2000).
 *   uint32_t __gj_batch_id_2000  (alias)
 *   uint32_t __gj_graph_milestone_2000  (alias)
 *   __libcgj_batch2000_marker = "libcgj-batch2000"
 *
 * Milestone for the product bar helpers exclusive pure-data wave
 * (batches 1991–1999: steam/deck/top50 capability bits, install stage
 * 0..15, soft/hard smoke gates, export count, libc.so.6 soname, dual
 * MIT/Apache license tag). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols —
 * avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2000_marker[] = "libcgj-batch2000";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2000_id(void)
{
	return 2000u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2000 — report this TU's graph batch number.
 *
 * Always returns 2000.
 */
uint32_t
gj_batch_id_2000(void)
{
	(void)NULL;
	return b2000_id();
}

/*
 * gj_graph_milestone_2000 — report this TU's graph milestone revision.
 *
 * Always returns 2000 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2000(void)
{
	return b2000_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2000(void)
    __attribute__((alias("gj_batch_id_2000")));

uint32_t __gj_graph_milestone_2000(void)
    __attribute__((alias("gj_graph_milestone_2000")));
