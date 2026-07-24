/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch26400: MILESTONE 26400 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_26400(void);
 *     - Returns the compile-time graph batch number for this TU (26400).
 *   uint32_t gj_graph_milestone_26400(void);
 *     - Returns the current graph milestone revision (26400).
 *   uint32_t __gj_batch_id_26400  (alias)
 *   uint32_t __gj_graph_milestone_26400  (alias)
 *   __libcgj_batch26400_marker = "libcgj-batch26400"
 *
 * MILESTONE 26400 for the exclusive continuum CREATE-ONLY wave
 * (batches 26391-26399: shell_green_26400, libcgj_green_26400,
 * bar3_ready_26400, product_score_26400, continuum_ready_26400,
 * smoke_soft_26400, dyn_soft_26400, milestone_tag_26400,
 * continuum_wave_26400). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=26400. bar3 remains open (gj_bar3_ready_26400 == 0);
 * product_score remains open (gj_product_score_26400 == 0).
 *
 * CGJ soft marker band capstone (26391–26400): host soft probes
 * (cgj_soft_milestone_26400.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 26400. */
const char __libcgj_batch26400_marker[] = "libcgj-batch26400";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b26400_id(void)
{
	return 26400u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_26400 - report this TU's graph batch number.
 *
 * Always returns 26400u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 26400.
 */
uint32_t
gj_batch_id_26400(void)
{
	(void)NULL;
	return b26400_id();
}

/*
 * gj_graph_milestone_26400 - report this TU's graph milestone revision.
 *
 * Always returns 26400u (MILESTONE 26400). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 26400.
 */
uint32_t
gj_graph_milestone_26400(void)
{
	return b26400_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_26400(void)
    __attribute__((alias("gj_batch_id_26400")));

uint32_t __gj_graph_milestone_26400(void)
    __attribute__((alias("gj_graph_milestone_26400")));
