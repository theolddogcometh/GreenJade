/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1930: milestone 1930 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_1930(void);
 *     — Returns the compile-time graph batch number for this TU (1930).
 *   uint32_t gj_graph_milestone_1930(void);
 *     — Returns the current graph milestone revision (1930).
 *   uint32_t __gj_batch_id_1930  (alias)
 *   uint32_t __gj_graph_milestone_1930  (alias)
 *   __libcgj_batch1930_marker = "libcgj-batch1930"
 *
 * Milestone for the network port/service exclusive pure-data wave
 * (batches 1921–1929). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols —
 * avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1930_marker[] = "libcgj-batch1930";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b1930_id(void)
{
	return 1930u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_1930 — report this TU's graph batch number.
 *
 * Always returns 1930.
 */
uint32_t
gj_batch_id_1930(void)
{
	(void)NULL;
	return b1930_id();
}

/*
 * gj_graph_milestone_1930 — report this TU's graph milestone revision.
 *
 * Always returns 1930 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_1930(void)
{
	return b1930_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_1930(void)
    __attribute__((alias("gj_batch_id_1930")));

uint32_t __gj_graph_milestone_1930(void)
    __attribute__((alias("gj_graph_milestone_1930")));
