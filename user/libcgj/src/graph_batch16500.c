/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch16500: MILESTONE 16500 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_16500(void);
 *     - Returns the compile-time graph batch number for this TU (16500).
 *   uint32_t gj_graph_milestone_16500(void);
 *     - Returns the current graph milestone revision (16500).
 *   uint32_t __gj_batch_id_16500  (alias)
 *   uint32_t __gj_graph_milestone_16500  (alias)
 *   __libcgj_batch16500_marker = "libcgj-batch16500"
 *
 * MILESTONE 16500 for the exclusive continuum CREATE-ONLY wave
 * (batches 16491-16499: shell_green_16500, libcgj_green_16500,
 * bar3_ready_16500, product_score_16500, continuum_ready_16500,
 * smoke_soft_16500, dyn_soft_16500, milestone_tag_16500,
 * continuum_wave_16500). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=16500. bar3 remains open (gj_bar3_ready_16500 == 0);
 * product_score remains open (gj_product_score_16500 == 0).
 *
 * CGJ soft marker band capstone (16491–16500): host soft probes
 * (cgj_soft_milestone_16500.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 16500. */
const char __libcgj_batch16500_marker[] = "libcgj-batch16500";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b16500_id(void)
{
	return 16500u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_16500 - report this TU's graph batch number.
 *
 * Always returns 16500u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 16500.
 */
uint32_t
gj_batch_id_16500(void)
{
	(void)NULL;
	return b16500_id();
}

/*
 * gj_graph_milestone_16500 - report this TU's graph milestone revision.
 *
 * Always returns 16500u (MILESTONE 16500). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 16500.
 */
uint32_t
gj_graph_milestone_16500(void)
{
	return b16500_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_16500(void)
    __attribute__((alias("gj_batch_id_16500")));

uint32_t __gj_graph_milestone_16500(void)
    __attribute__((alias("gj_graph_milestone_16500")));
