/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch40200: MILESTONE 40200 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_40200(void);
 *     - Returns the compile-time graph batch number for this TU (40200).
 *   uint32_t gj_graph_milestone_40200(void);
 *     - Returns the current graph milestone revision (40200).
 *   uint32_t __gj_batch_id_40200  (alias)
 *   uint32_t __gj_graph_milestone_40200  (alias)
 *   __libcgj_batch40200_marker = "libcgj-batch40200"
 *
 * MILESTONE 40200 for the exclusive continuum CREATE-ONLY wave
 * (batches 40191-40199: shell_green_40200, libcgj_green_40200,
 * bar3_ready_40200, product_score_40200, continuum_ready_40200,
 * smoke_soft_40200, dyn_soft_40200, milestone_tag_40200,
 * continuum_wave_40200). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=40200. bar3 remains open (gj_bar3_ready_40200 == 0);
 * product_score remains open (gj_product_score_40200 == 0).
 *
 * CGJ soft marker band capstone (40191–40200): host soft probes
 * (cgj_soft_milestone_40200.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 40200. */
const char __libcgj_batch40200_marker[] = "libcgj-batch40200";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b40200_id(void)
{
	return 40200u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_40200 - report this TU's graph batch number.
 *
 * Always returns 40200u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 40200.
 */
uint32_t
gj_batch_id_40200(void)
{
	(void)NULL;
	return b40200_id();
}

/*
 * gj_graph_milestone_40200 - report this TU's graph milestone revision.
 *
 * Always returns 40200u (MILESTONE 40200). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 40200.
 */
uint32_t
gj_graph_milestone_40200(void)
{
	return b40200_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_40200(void)
    __attribute__((alias("gj_batch_id_40200")));

uint32_t __gj_graph_milestone_40200(void)
    __attribute__((alias("gj_graph_milestone_40200")));
