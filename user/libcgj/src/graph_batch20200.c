/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch20200: MILESTONE 20200 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_20200(void);
 *     - Returns the compile-time graph batch number for this TU (20200).
 *   uint32_t gj_graph_milestone_20200(void);
 *     - Returns the current graph milestone revision (20200).
 *   uint32_t __gj_batch_id_20200  (alias)
 *   uint32_t __gj_graph_milestone_20200  (alias)
 *   __libcgj_batch20200_marker = "libcgj-batch20200"
 *
 * MILESTONE 20200 for the exclusive continuum CREATE-ONLY wave
 * (batches 20191-20199: shell_green_20200, libcgj_green_20200,
 * bar3_ready_20200, product_score_20200, continuum_ready_20200,
 * smoke_soft_20200, dyn_soft_20200, milestone_tag_20200,
 * continuum_wave_20200). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=20200. bar3 remains open (gj_bar3_ready_20200 == 0);
 * product_score remains open (gj_product_score_20200 == 0).
 *
 * CGJ soft marker band capstone (20191–20200): host soft probes
 * (cgj_soft_milestone_20200.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 20200. */
const char __libcgj_batch20200_marker[] = "libcgj-batch20200";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b20200_id(void)
{
	return 20200u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_20200 - report this TU's graph batch number.
 *
 * Always returns 20200u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 20200.
 */
uint32_t
gj_batch_id_20200(void)
{
	(void)NULL;
	return b20200_id();
}

/*
 * gj_graph_milestone_20200 - report this TU's graph milestone revision.
 *
 * Always returns 20200u (MILESTONE 20200). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 20200.
 */
uint32_t
gj_graph_milestone_20200(void)
{
	return b20200_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_20200(void)
    __attribute__((alias("gj_batch_id_20200")));

uint32_t __gj_graph_milestone_20200(void)
    __attribute__((alias("gj_graph_milestone_20200")));
