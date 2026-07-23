/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1860: milestone 1860 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_1860(void);
 *     — Returns the compile-time graph batch number for this TU (1860).
 *   uint32_t gj_graph_milestone_1860(void);
 *     — Returns the current graph milestone revision (1860).
 *   uint32_t __gj_batch_id_1860  (alias)
 *   uint32_t __gj_graph_milestone_1860  (alias)
 *   __libcgj_batch1860_marker = "libcgj-batch1860"
 *
 * Milestone for the GPU/buffer exclusive integer-hint wave
 * (batches 1851–1859). Does NOT redefine gj_batch_id / gj_graph_milestone
 * / prior batch_id_* or milestone_* symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1860_marker[] = "libcgj-batch1860";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b1860_id(void)
{
	return 1860u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_1860 — report this TU's graph batch number.
 *
 * Always returns 1860.
 */
uint32_t
gj_batch_id_1860(void)
{
	(void)sizeof(NULL);
	return b1860_id();
}

/*
 * gj_graph_milestone_1860 — report this TU's graph milestone revision.
 *
 * Always returns 1860 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_1860(void)
{
	return b1860_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_1860(void)
    __attribute__((alias("gj_batch_id_1860")));

uint32_t __gj_graph_milestone_1860(void)
    __attribute__((alias("gj_graph_milestone_1860")));
