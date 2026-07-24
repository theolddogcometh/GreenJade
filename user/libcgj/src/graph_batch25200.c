/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch25200: MILESTONE 25200 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_25200(void);
 *     - Returns the compile-time graph batch number for this TU (25200).
 *   uint32_t gj_graph_milestone_25200(void);
 *     - Returns the current graph milestone revision (25200).
 *   uint32_t __gj_batch_id_25200  (alias)
 *   uint32_t __gj_graph_milestone_25200  (alias)
 *   __libcgj_batch25200_marker = "libcgj-batch25200"
 *
 * MILESTONE 25200 for the exclusive continuum CREATE-ONLY wave
 * (batches 25191-25199: shell_green_25200, libcgj_green_25200,
 * bar3_ready_25200, product_score_25200, continuum_ready_25200,
 * smoke_soft_25200, dyn_soft_25200, milestone_tag_25200,
 * continuum_wave_25200). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=25200. bar3 remains open (gj_bar3_ready_25200 == 0);
 * product_score remains open (gj_product_score_25200 == 0).
 *
 * CGJ soft marker band capstone (25191–25200): host soft probes
 * (cgj_soft_milestone_25200.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 25200. */
const char __libcgj_batch25200_marker[] = "libcgj-batch25200";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b25200_id(void)
{
	return 25200u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_25200 - report this TU's graph batch number.
 *
 * Always returns 25200u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 25200.
 */
uint32_t
gj_batch_id_25200(void)
{
	(void)NULL;
	return b25200_id();
}

/*
 * gj_graph_milestone_25200 - report this TU's graph milestone revision.
 *
 * Always returns 25200u (MILESTONE 25200). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 25200.
 */
uint32_t
gj_graph_milestone_25200(void)
{
	return b25200_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_25200(void)
    __attribute__((alias("gj_batch_id_25200")));

uint32_t __gj_graph_milestone_25200(void)
    __attribute__((alias("gj_graph_milestone_25200")));
