/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch95500: MILESTONE 95500 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_95500(void);
 *     - Returns the compile-time graph batch number for this TU (95500).
 *   uint32_t gj_graph_milestone_95500(void);
 *     - Returns the current graph milestone revision (95500).
 *   uint32_t __gj_batch_id_95500  (alias)
 *   uint32_t __gj_graph_milestone_95500  (alias)
 *   __libcgj_batch95500_marker = "libcgj-batch95500"
 *
 * MILESTONE 95500 for the exclusive continuum CREATE-ONLY wave
 * (batches 25891-25899: shell_green_95500, libcgj_green_95500,
 * bar3_ready_95500, product_score_95500, continuum_ready_95500,
 * smoke_soft_95500, dyn_soft_95500, milestone_tag_95500,
 * continuum_wave_95500). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=95500. bar3 remains open (gj_bar3_ready_95500 == 0);
 * product_score remains open (gj_product_score_95500 == 0).
 *
 * CGJ soft marker band capstone (25891–95500): host soft probes
 * (cgj_soft_milestone_95500.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 95500. */
const char __libcgj_batch95500_marker[] = "libcgj-batch95500";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b95500_id(void)
{
	return 95500u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_95500 - report this TU's graph batch number.
 *
 * Always returns 95500u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 95500.
 */
uint32_t
gj_batch_id_95500(void)
{
	(void)NULL;
	return b95500_id();
}

/*
 * gj_graph_milestone_95500 - report this TU's graph milestone revision.
 *
 * Always returns 95500u (MILESTONE 95500). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 95500.
 */
uint32_t
gj_graph_milestone_95500(void)
{
	return b95500_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_95500(void)
    __attribute__((alias("gj_batch_id_95500")));

uint32_t __gj_graph_milestone_95500(void)
    __attribute__((alias("gj_graph_milestone_95500")));
