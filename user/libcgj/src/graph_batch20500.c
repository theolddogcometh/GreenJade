/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch20500: MILESTONE 20500 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_20500(void);
 *     - Returns the compile-time graph batch number for this TU (20500).
 *   uint32_t gj_graph_milestone_20500(void);
 *     - Returns the current graph milestone revision (20500).
 *   uint32_t __gj_batch_id_20500  (alias)
 *   uint32_t __gj_graph_milestone_20500  (alias)
 *   __libcgj_batch20500_marker = "libcgj-batch20500"
 *
 * MILESTONE 20500 for the exclusive continuum CREATE-ONLY wave
 * (batches 20491-20499: shell_green_20500, libcgj_green_20500,
 * bar3_ready_20500, product_score_20500, continuum_ready_20500,
 * smoke_soft_20500, dyn_soft_20500, milestone_tag_20500,
 * continuum_wave_20500). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=20500. bar3 remains open (gj_bar3_ready_20500 == 0);
 * product_score remains open (gj_product_score_20500 == 0).
 *
 * CGJ soft marker band capstone (20491–20500): host soft probes
 * (cgj_soft_milestone_20500.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 20500. */
const char __libcgj_batch20500_marker[] = "libcgj-batch20500";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b20500_id(void)
{
	return 20500u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_20500 - report this TU's graph batch number.
 *
 * Always returns 20500u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 20500.
 */
uint32_t
gj_batch_id_20500(void)
{
	(void)NULL;
	return b20500_id();
}

/*
 * gj_graph_milestone_20500 - report this TU's graph milestone revision.
 *
 * Always returns 20500u (MILESTONE 20500). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 20500.
 */
uint32_t
gj_graph_milestone_20500(void)
{
	return b20500_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_20500(void)
    __attribute__((alias("gj_batch_id_20500")));

uint32_t __gj_graph_milestone_20500(void)
    __attribute__((alias("gj_graph_milestone_20500")));
