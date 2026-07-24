/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch22400: MILESTONE 22400 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_22400(void);
 *     - Returns the compile-time graph batch number for this TU (22400).
 *   uint32_t gj_graph_milestone_22400(void);
 *     - Returns the current graph milestone revision (22400).
 *   uint32_t __gj_batch_id_22400  (alias)
 *   uint32_t __gj_graph_milestone_22400  (alias)
 *   __libcgj_batch22400_marker = "libcgj-batch22400"
 *
 * MILESTONE 22400 for the exclusive continuum CREATE-ONLY wave
 * (batches 22391-22399: shell_green_22400, libcgj_green_22400,
 * bar3_ready_22400, product_score_22400, continuum_ready_22400,
 * smoke_soft_22400, dyn_soft_22400, milestone_tag_22400,
 * continuum_wave_22400). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=22400. bar3 remains open (gj_bar3_ready_22400 == 0);
 * product_score remains open (gj_product_score_22400 == 0).
 *
 * CGJ soft marker band capstone (22391–22400): host soft probes
 * (cgj_soft_milestone_22400.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 22400. */
const char __libcgj_batch22400_marker[] = "libcgj-batch22400";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b22400_id(void)
{
	return 22400u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_22400 - report this TU's graph batch number.
 *
 * Always returns 22400u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 22400.
 */
uint32_t
gj_batch_id_22400(void)
{
	(void)NULL;
	return b22400_id();
}

/*
 * gj_graph_milestone_22400 - report this TU's graph milestone revision.
 *
 * Always returns 22400u (MILESTONE 22400). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 22400.
 */
uint32_t
gj_graph_milestone_22400(void)
{
	return b22400_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_22400(void)
    __attribute__((alias("gj_batch_id_22400")));

uint32_t __gj_graph_milestone_22400(void)
    __attribute__((alias("gj_graph_milestone_22400")));
