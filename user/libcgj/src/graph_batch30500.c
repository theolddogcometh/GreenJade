/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch30500: MILESTONE 30500 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_30500(void);
 *     - Returns the compile-time graph batch number for this TU (30500).
 *   uint32_t gj_graph_milestone_30500(void);
 *     - Returns the current graph milestone revision (30500).
 *   uint32_t __gj_batch_id_30500  (alias)
 *   uint32_t __gj_graph_milestone_30500  (alias)
 *   __libcgj_batch30500_marker = "libcgj-batch30500"
 *
 * MILESTONE 30500 for the exclusive continuum CREATE-ONLY wave
 * (batches 30491-30499: shell_green_30500, libcgj_green_30500,
 * bar3_ready_30500, product_score_30500, continuum_ready_30500,
 * smoke_soft_30500, dyn_soft_30500, milestone_tag_30500,
 * continuum_wave_30500). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=30500. bar3 remains open (gj_bar3_ready_30500 == 0);
 * product_score remains open (gj_product_score_30500 == 0).
 *
 * CGJ soft marker band capstone (30491–30500): host soft probes
 * (cgj_soft_milestone_30500.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 30500. */
const char __libcgj_batch30500_marker[] = "libcgj-batch30500";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b30500_id(void)
{
	return 30500u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_30500 - report this TU's graph batch number.
 *
 * Always returns 30500u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 30500.
 */
uint32_t
gj_batch_id_30500(void)
{
	(void)NULL;
	return b30500_id();
}

/*
 * gj_graph_milestone_30500 - report this TU's graph milestone revision.
 *
 * Always returns 30500u (MILESTONE 30500). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 30500.
 */
uint32_t
gj_graph_milestone_30500(void)
{
	return b30500_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_30500(void)
    __attribute__((alias("gj_batch_id_30500")));

uint32_t __gj_graph_milestone_30500(void)
    __attribute__((alias("gj_graph_milestone_30500")));
