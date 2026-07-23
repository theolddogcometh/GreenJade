/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12700: MILESTONE 12700 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_12700(void);
 *     - Returns the compile-time graph batch number for this TU (12700).
 *   uint32_t gj_graph_milestone_12700(void);
 *     - Returns the current graph milestone revision (12700).
 *   uint32_t __gj_batch_id_12700  (alias)
 *   uint32_t __gj_graph_milestone_12700  (alias)
 *   __libcgj_batch12700_marker = "libcgj-batch12700"
 *
 * MILESTONE 12700 for the exclusive continuum CREATE-ONLY wave
 * (batches 12691-12699: shell_green_12700, libcgj_green_12700,
 * bar3_ready_12700, product_score_12700, continuum_ready_12700,
 * smoke_soft_12700, dyn_soft_12700, milestone_tag_12700,
 * continuum_wave_12700). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols -
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12700_marker[] = "libcgj-batch12700";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12700_id(void)
{
	return 12700u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_12700 - report this TU's graph batch number.
 *
 * Always returns 12700.
 */
uint32_t
gj_batch_id_12700(void)
{
	(void)NULL;
	return b12700_id();
}

/*
 * gj_graph_milestone_12700 - report this TU's graph milestone revision.
 *
 * Always returns 12700 (MILESTONE 12700). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_12700(void)
{
	return b12700_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_12700(void)
    __attribute__((alias("gj_batch_id_12700")));

uint32_t __gj_graph_milestone_12700(void)
    __attribute__((alias("gj_graph_milestone_12700")));
