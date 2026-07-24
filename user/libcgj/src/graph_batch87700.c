/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch87700: MILESTONE 87700 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_87700(void);
 *     - Returns the compile-time graph batch number for this TU (87700).
 *   uint32_t gj_graph_milestone_87700(void);
 *     - Returns the current graph milestone revision (87700).
 *   uint32_t __gj_batch_id_87700  (alias)
 *   uint32_t __gj_graph_milestone_87700  (alias)
 *   __libcgj_batch87700_marker = "libcgj-batch87700"
 *
 * MILESTONE 87700 for the exclusive continuum CREATE-ONLY wave
 * (batches 25891-25899: shell_green_87700, libcgj_green_87700,
 * bar3_ready_87700, product_score_87700, continuum_ready_87700,
 * smoke_soft_87700, dyn_soft_87700, milestone_tag_87700,
 * continuum_wave_87700). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=87700. bar3 remains open (gj_bar3_ready_87700 == 0);
 * product_score remains open (gj_product_score_87700 == 0).
 *
 * CGJ soft marker band capstone (25891–87700): host soft probes
 * (cgj_soft_milestone_87700.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 87700. */
const char __libcgj_batch87700_marker[] = "libcgj-batch87700";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b87700_id(void)
{
	return 87700u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_87700 - report this TU's graph batch number.
 *
 * Always returns 87700u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 87700.
 */
uint32_t
gj_batch_id_87700(void)
{
	(void)NULL;
	return b87700_id();
}

/*
 * gj_graph_milestone_87700 - report this TU's graph milestone revision.
 *
 * Always returns 87700u (MILESTONE 87700). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 87700.
 */
uint32_t
gj_graph_milestone_87700(void)
{
	return b87700_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_87700(void)
    __attribute__((alias("gj_batch_id_87700")));

uint32_t __gj_graph_milestone_87700(void)
    __attribute__((alias("gj_graph_milestone_87700")));
