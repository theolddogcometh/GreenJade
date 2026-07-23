/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1980: milestone 1980 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_1980(void);
 *     — Returns the compile-time graph batch number for this TU (1980).
 *   uint32_t gj_graph_milestone_1980(void);
 *     — Returns the current graph milestone revision (1980).
 *   uint32_t __gj_batch_id_1980  (alias)
 *   uint32_t __gj_graph_milestone_1980  (alias)
 *   __libcgj_batch1980_marker = "libcgj-batch1980"
 *
 * Milestone for the time-duration exclusive pure-data wave
 * (batches 1971–1979). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols —
 * avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1980_marker[] = "libcgj-batch1980";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b1980_id(void)
{
	return 1980u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_1980 — report this TU's graph batch number.
 *
 * Always returns 1980.
 */
uint32_t
gj_batch_id_1980(void)
{
	(void)NULL;
	return b1980_id();
}

/*
 * gj_graph_milestone_1980 — report this TU's graph milestone revision.
 *
 * Always returns 1980 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_1980(void)
{
	return b1980_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_1980(void)
    __attribute__((alias("gj_batch_id_1980")));

uint32_t __gj_graph_milestone_1980(void)
    __attribute__((alias("gj_graph_milestone_1980")));
