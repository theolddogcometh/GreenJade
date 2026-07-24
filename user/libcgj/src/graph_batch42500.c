/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch42500: MILESTONE 42500 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_42500(void);
 *     - Returns the compile-time graph batch number for this TU (42500).
 *   uint32_t gj_graph_milestone_42500(void);
 *     - Returns the current graph milestone revision (42500).
 *   uint32_t __gj_batch_id_42500  (alias)
 *   uint32_t __gj_graph_milestone_42500  (alias)
 *   __libcgj_batch42500_marker = "libcgj-batch42500"
 *
 * MILESTONE 42500 for the exclusive continuum CREATE-ONLY wave
 * (batches 42491-42499: shell_green_42500, libcgj_green_42500,
 * bar3_ready_42500, product_score_42500, continuum_ready_42500,
 * smoke_soft_42500, dyn_soft_42500, milestone_tag_42500,
 * continuum_wave_42500). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=42500. bar3 remains open (gj_bar3_ready_42500 == 0);
 * product_score remains open (gj_product_score_42500 == 0).
 *
 * CGJ soft marker band capstone (42491–42500): host soft probes
 * (cgj_soft_milestone_42500.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 42500. */
const char __libcgj_batch42500_marker[] = "libcgj-batch42500";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b42500_id(void)
{
	return 42500u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_42500 - report this TU's graph batch number.
 *
 * Always returns 42500u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 42500.
 */
uint32_t
gj_batch_id_42500(void)
{
	(void)NULL;
	return b42500_id();
}

/*
 * gj_graph_milestone_42500 - report this TU's graph milestone revision.
 *
 * Always returns 42500u (MILESTONE 42500). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 42500.
 */
uint32_t
gj_graph_milestone_42500(void)
{
	return b42500_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_42500(void)
    __attribute__((alias("gj_batch_id_42500")));

uint32_t __gj_graph_milestone_42500(void)
    __attribute__((alias("gj_graph_milestone_42500")));
