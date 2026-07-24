/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch17400: MILESTONE 17400 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_17400(void);
 *     - Returns the compile-time graph batch number for this TU (17400).
 *   uint32_t gj_graph_milestone_17400(void);
 *     - Returns the current graph milestone revision (17400).
 *   uint32_t __gj_batch_id_17400  (alias)
 *   uint32_t __gj_graph_milestone_17400  (alias)
 *   __libcgj_batch17400_marker = "libcgj-batch17400"
 *
 * MILESTONE 17400 for the exclusive continuum CREATE-ONLY wave
 * (batches 17391-17399: shell_green_17400, libcgj_green_17400,
 * bar3_ready_17400, product_score_17400, continuum_ready_17400,
 * smoke_soft_17400, dyn_soft_17400, milestone_tag_17400,
 * continuum_wave_17400). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=17400. bar3 remains open (gj_bar3_ready_17400 == 0);
 * product_score remains open (gj_product_score_17400 == 0).
 *
 * CGJ soft marker band capstone (17391–17400): host soft probes
 * (cgj_soft_milestone_17400.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 17400. */
const char __libcgj_batch17400_marker[] = "libcgj-batch17400";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b17400_id(void)
{
	return 17400u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_17400 - report this TU's graph batch number.
 *
 * Always returns 17400u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 17400.
 */
uint32_t
gj_batch_id_17400(void)
{
	(void)NULL;
	return b17400_id();
}

/*
 * gj_graph_milestone_17400 - report this TU's graph milestone revision.
 *
 * Always returns 17400u (MILESTONE 17400). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 17400.
 */
uint32_t
gj_graph_milestone_17400(void)
{
	return b17400_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_17400(void)
    __attribute__((alias("gj_batch_id_17400")));

uint32_t __gj_graph_milestone_17400(void)
    __attribute__((alias("gj_graph_milestone_17400")));
