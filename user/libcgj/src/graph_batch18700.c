/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch18700: MILESTONE 18700 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_18700(void);
 *     - Returns the compile-time graph batch number for this TU (18700).
 *   uint32_t gj_graph_milestone_18700(void);
 *     - Returns the current graph milestone revision (18700).
 *   uint32_t __gj_batch_id_18700  (alias)
 *   uint32_t __gj_graph_milestone_18700  (alias)
 *   __libcgj_batch18700_marker = "libcgj-batch18700"
 *
 * MILESTONE 18700 for the exclusive continuum CREATE-ONLY wave
 * (batches 18691-18699: shell_green_18700, libcgj_green_18700,
 * bar3_ready_18700, product_score_18700, continuum_ready_18700,
 * smoke_soft_18700, dyn_soft_18700, milestone_tag_18700,
 * continuum_wave_18700). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=18700. bar3 remains open (gj_bar3_ready_18700 == 0);
 * product_score remains open (gj_product_score_18700 == 0).
 *
 * CGJ soft marker band capstone (18691–18700): host soft probes
 * (cgj_soft_milestone_18700.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 18700. */
const char __libcgj_batch18700_marker[] = "libcgj-batch18700";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b18700_id(void)
{
	return 18700u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_18700 - report this TU's graph batch number.
 *
 * Always returns 18700u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 18700.
 */
uint32_t
gj_batch_id_18700(void)
{
	(void)NULL;
	return b18700_id();
}

/*
 * gj_graph_milestone_18700 - report this TU's graph milestone revision.
 *
 * Always returns 18700u (MILESTONE 18700). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 18700.
 */
uint32_t
gj_graph_milestone_18700(void)
{
	return b18700_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_18700(void)
    __attribute__((alias("gj_batch_id_18700")));

uint32_t __gj_graph_milestone_18700(void)
    __attribute__((alias("gj_graph_milestone_18700")));
