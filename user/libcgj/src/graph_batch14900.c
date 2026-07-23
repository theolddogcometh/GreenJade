/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14900: MILESTONE 14900 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14900(void);
 *     - Returns the compile-time graph batch number for this TU (14900).
 *   uint32_t gj_graph_milestone_14900(void);
 *     - Returns the current graph milestone revision (14900).
 *   uint32_t __gj_batch_id_14900  (alias)
 *   uint32_t __gj_graph_milestone_14900  (alias)
 *   __libcgj_batch14900_marker = "libcgj-batch14900"
 *
 * MILESTONE 14900 for the exclusive continuum CREATE-ONLY wave
 * (batches 14891-14899: shell_green_14900, libcgj_green_14900,
 * bar3_ready_14900, product_score_14900, continuum_ready_14900,
 * smoke_soft_14900, dyn_soft_14900, milestone_tag_14900,
 * continuum_wave_14900). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14900_marker[] = "libcgj-batch14900";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14900_id(void)
{
	return 14900u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14900 - report this TU's graph batch number.
 *
 * Always returns 14900.
 */
uint32_t
gj_batch_id_14900(void)
{
	(void)NULL;
	return b14900_id();
}

/*
 * gj_graph_milestone_14900 - report this TU's graph milestone revision.
 *
 * Always returns 14900 (MILESTONE 14900). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_14900(void)
{
	return b14900_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14900(void)
    __attribute__((alias("gj_batch_id_14900")));

uint32_t __gj_graph_milestone_14900(void)
    __attribute__((alias("gj_graph_milestone_14900")));
