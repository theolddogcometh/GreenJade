/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch24500: MILESTONE 24500 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_24500(void);
 *     - Returns the compile-time graph batch number for this TU (24500).
 *   uint32_t gj_graph_milestone_24500(void);
 *     - Returns the current graph milestone revision (24500).
 *   uint32_t __gj_batch_id_24500  (alias)
 *   uint32_t __gj_graph_milestone_24500  (alias)
 *   __libcgj_batch24500_marker = "libcgj-batch24500"
 *
 * MILESTONE 24500 for the exclusive continuum CREATE-ONLY wave
 * (batches 24491-24499: shell_green_24500, libcgj_green_24500,
 * bar3_ready_24500, product_score_24500, continuum_ready_24500,
 * smoke_soft_24500, dyn_soft_24500, milestone_tag_24500,
 * continuum_wave_24500). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=24500. bar3 remains open (gj_bar3_ready_24500 == 0);
 * product_score remains open (gj_product_score_24500 == 0).
 *
 * CGJ soft marker band capstone (24491–24500): host soft probes
 * (cgj_soft_milestone_24500.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 24500. */
const char __libcgj_batch24500_marker[] = "libcgj-batch24500";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b24500_id(void)
{
	return 24500u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_24500 - report this TU's graph batch number.
 *
 * Always returns 24500u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 24500.
 */
uint32_t
gj_batch_id_24500(void)
{
	(void)NULL;
	return b24500_id();
}

/*
 * gj_graph_milestone_24500 - report this TU's graph milestone revision.
 *
 * Always returns 24500u (MILESTONE 24500). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 24500.
 */
uint32_t
gj_graph_milestone_24500(void)
{
	return b24500_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_24500(void)
    __attribute__((alias("gj_batch_id_24500")));

uint32_t __gj_graph_milestone_24500(void)
    __attribute__((alias("gj_graph_milestone_24500")));
