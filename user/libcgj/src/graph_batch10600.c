/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10600: MILESTONE 10600 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10600(void);
 *     - Returns the compile-time graph batch number for this TU (10600).
 *   uint32_t gj_graph_milestone_10600(void);
 *     - Returns the current graph milestone revision (10600).
 *   uint32_t __gj_batch_id_10600  (alias)
 *   uint32_t __gj_graph_milestone_10600  (alias)
 *   __libcgj_batch10600_marker = "libcgj-batch10600"
 *
 * MILESTONE 10600 for the exclusive continuum CREATE-ONLY wave
 * (batches 10591-10599: shell_green_10600, libcgj_green_10600,
 * bar3_ready_10600, product_score_10600, continuum_ready_10600,
 * smoke_soft_10600, dyn_soft_10600, milestone_tag_10600,
 * continuum_wave_10600). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols -
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10600_marker[] = "libcgj-batch10600";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10600_id(void)
{
	return 10600u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10600 - report this TU's graph batch number.
 *
 * Always returns 10600.
 */
uint32_t
gj_batch_id_10600(void)
{
	(void)NULL;
	return b10600_id();
}

/*
 * gj_graph_milestone_10600 - report this TU's graph milestone revision.
 *
 * Always returns 10600 (MILESTONE 10600). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_10600(void)
{
	return b10600_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_10600(void)
    __attribute__((alias("gj_batch_id_10600")));

uint32_t __gj_graph_milestone_10600(void)
    __attribute__((alias("gj_graph_milestone_10600")));
