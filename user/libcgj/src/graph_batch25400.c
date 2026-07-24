/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch25400: MILESTONE 25400 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_25400(void);
 *     - Returns the compile-time graph batch number for this TU (25400).
 *   uint32_t gj_graph_milestone_25400(void);
 *     - Returns the current graph milestone revision (25400).
 *   uint32_t __gj_batch_id_25400  (alias)
 *   uint32_t __gj_graph_milestone_25400  (alias)
 *   __libcgj_batch25400_marker = "libcgj-batch25400"
 *
 * MILESTONE 25400 for the exclusive continuum CREATE-ONLY wave
 * (batches 25391-25399: shell_green_25400, libcgj_green_25400,
 * bar3_ready_25400, product_score_25400, continuum_ready_25400,
 * smoke_soft_25400, dyn_soft_25400, milestone_tag_25400,
 * continuum_wave_25400). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=25400. bar3 remains open (gj_bar3_ready_25400 == 0);
 * product_score remains open (gj_product_score_25400 == 0).
 *
 * CGJ soft marker band capstone (25391–25400): host soft probes
 * (cgj_soft_milestone_25400.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 25400. */
const char __libcgj_batch25400_marker[] = "libcgj-batch25400";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b25400_id(void)
{
	return 25400u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_25400 - report this TU's graph batch number.
 *
 * Always returns 25400u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 25400.
 */
uint32_t
gj_batch_id_25400(void)
{
	(void)NULL;
	return b25400_id();
}

/*
 * gj_graph_milestone_25400 - report this TU's graph milestone revision.
 *
 * Always returns 25400u (MILESTONE 25400). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 25400.
 */
uint32_t
gj_graph_milestone_25400(void)
{
	return b25400_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_25400(void)
    __attribute__((alias("gj_batch_id_25400")));

uint32_t __gj_graph_milestone_25400(void)
    __attribute__((alias("gj_graph_milestone_25400")));
