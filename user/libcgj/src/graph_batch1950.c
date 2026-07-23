/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1950: milestone 1950 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_1950(void);
 *     — Returns the compile-time graph batch number for this TU (1950).
 *   uint32_t gj_graph_milestone_1950(void);
 *     — Returns the current graph milestone revision (1950).
 *   uint32_t __gj_batch_id_1950  (alias)
 *   uint32_t __gj_graph_milestone_1950  (alias)
 *   __libcgj_batch1950_marker = "libcgj-batch1950"
 *
 * Milestone for the logging/trace exclusive pure-data wave
 * (batches 1941–1949). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols —
 * avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1950_marker[] = "libcgj-batch1950";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b1950_id(void)
{
	return 1950u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_1950 — report this TU's graph batch number.
 *
 * Always returns 1950.
 */
uint32_t
gj_batch_id_1950(void)
{
	(void)NULL;
	return b1950_id();
}

/*
 * gj_graph_milestone_1950 — report this TU's graph milestone revision.
 *
 * Always returns 1950 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_1950(void)
{
	return b1950_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_1950(void)
    __attribute__((alias("gj_batch_id_1950")));

uint32_t __gj_graph_milestone_1950(void)
    __attribute__((alias("gj_graph_milestone_1950")));
