/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9600: MILESTONE 9600 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9600(void);
 *     - Returns the compile-time graph batch number for this TU (9600).
 *   uint32_t gj_graph_milestone_9600(void);
 *     - Returns the current graph milestone revision (9600).
 *   uint32_t __gj_batch_id_9600  (alias)
 *   uint32_t __gj_graph_milestone_9600  (alias)
 *   __libcgj_batch9600_marker = "libcgj-batch9600"
 *
 * MILESTONE 9600 for the exclusive continuum CREATE-ONLY wave
 * (batches 9591-9599: shell_green_9600, libcgj_green_9600,
 * bar3_ready_9600, product_score_9600, continuum_ready_9600,
 * smoke_soft_9600, dyn_soft_9600, milestone_tag_9600,
 * continuum_wave_9600). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols -
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9600_marker[] = "libcgj-batch9600";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9600_id(void)
{
	return 9600u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9600 - report this TU's graph batch number.
 *
 * Always returns 9600.
 */
uint32_t
gj_batch_id_9600(void)
{
	(void)NULL;
	return b9600_id();
}

/*
 * gj_graph_milestone_9600 - report this TU's graph milestone revision.
 *
 * Always returns 9600 (MILESTONE 9600). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_9600(void)
{
	return b9600_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_9600(void)
    __attribute__((alias("gj_batch_id_9600")));

uint32_t __gj_graph_milestone_9600(void)
    __attribute__((alias("gj_graph_milestone_9600")));
