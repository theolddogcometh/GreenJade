/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1890: milestone 1890 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_1890(void);
 *     — Returns the compile-time graph batch number for this TU (1890).
 *   uint32_t gj_graph_milestone_1890(void);
 *     — Returns the current graph milestone revision (1890).
 *   uint32_t __gj_batch_id_1890  (alias)
 *   uint32_t __gj_graph_milestone_1890  (alias)
 *   __libcgj_batch1890_marker = "libcgj-batch1890"
 *
 * Milestone for the signal/errno exclusive pure-data wave
 * (batches 1881–1889). Does NOT redefine gj_batch_id / gj_graph_milestone
 * / prior batch_id_* or milestone_* symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1890_marker[] = "libcgj-batch1890";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b1890_id(void)
{
	(void)NULL;
	return 1890u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_1890 — report this TU's graph batch number.
 *
 * Always returns 1890.
 */
uint32_t
gj_batch_id_1890(void)
{
	return b1890_id();
}

/*
 * gj_graph_milestone_1890 — report this TU's graph milestone revision.
 *
 * Always returns 1890 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_1890(void)
{
	return b1890_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_1890(void)
    __attribute__((alias("gj_batch_id_1890")));

uint32_t __gj_graph_milestone_1890(void)
    __attribute__((alias("gj_graph_milestone_1890")));
