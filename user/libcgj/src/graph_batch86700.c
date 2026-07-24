/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch86700: MILESTONE 86700 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_86700(void);
 *     - Returns the compile-time graph batch number for this TU (86700).
 *   uint32_t gj_graph_milestone_86700(void);
 *     - Returns the current graph milestone revision (86700).
 *   uint32_t __gj_batch_id_86700  (alias)
 *   uint32_t __gj_graph_milestone_86700  (alias)
 *   __libcgj_batch86700_marker = "libcgj-batch86700"
 *
 * MILESTONE 86700 for the exclusive continuum CREATE-ONLY wave
 * (batches 25891-25899: shell_green_86700, libcgj_green_86700,
 * bar3_ready_86700, product_score_86700, continuum_ready_86700,
 * smoke_soft_86700, dyn_soft_86700, milestone_tag_86700,
 * continuum_wave_86700). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=86700. bar3 remains open (gj_bar3_ready_86700 == 0);
 * product_score remains open (gj_product_score_86700 == 0).
 *
 * CGJ soft marker band capstone (25891–86700): host soft probes
 * (cgj_soft_milestone_86700.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 86700. */
const char __libcgj_batch86700_marker[] = "libcgj-batch86700";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b86700_id(void)
{
	return 86700u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_86700 - report this TU's graph batch number.
 *
 * Always returns 86700u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 86700.
 */
uint32_t
gj_batch_id_86700(void)
{
	(void)NULL;
	return b86700_id();
}

/*
 * gj_graph_milestone_86700 - report this TU's graph milestone revision.
 *
 * Always returns 86700u (MILESTONE 86700). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 86700.
 */
uint32_t
gj_graph_milestone_86700(void)
{
	return b86700_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_86700(void)
    __attribute__((alias("gj_batch_id_86700")));

uint32_t __gj_graph_milestone_86700(void)
    __attribute__((alias("gj_graph_milestone_86700")));
