/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch48500: MILESTONE 48500 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_48500(void);
 *     - Returns the compile-time graph batch number for this TU (48500).
 *   uint32_t gj_graph_milestone_48500(void);
 *     - Returns the current graph milestone revision (48500).
 *   uint32_t __gj_batch_id_48500  (alias)
 *   uint32_t __gj_graph_milestone_48500  (alias)
 *   __libcgj_batch48500_marker = "libcgj-batch48500"
 *
 * MILESTONE 48500 for the exclusive continuum CREATE-ONLY wave
 * (batches 48491-48499: shell_green_48500, libcgj_green_48500,
 * bar3_ready_48500, product_score_48500, continuum_ready_48500,
 * smoke_soft_48500, dyn_soft_48500, milestone_tag_48500,
 * continuum_wave_48500). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=48500. bar3 remains open (gj_bar3_ready_48500 == 0);
 * product_score remains open (gj_product_score_48500 == 0).
 *
 * CGJ soft marker band capstone (48491–48500): host soft probes
 * (cgj_soft_milestone_48500.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 48500. */
const char __libcgj_batch48500_marker[] = "libcgj-batch48500";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b48500_id(void)
{
	return 48500u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_48500 - report this TU's graph batch number.
 *
 * Always returns 48500u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 48500.
 */
uint32_t
gj_batch_id_48500(void)
{
	(void)NULL;
	return b48500_id();
}

/*
 * gj_graph_milestone_48500 - report this TU's graph milestone revision.
 *
 * Always returns 48500u (MILESTONE 48500). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 48500.
 */
uint32_t
gj_graph_milestone_48500(void)
{
	return b48500_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_48500(void)
    __attribute__((alias("gj_batch_id_48500")));

uint32_t __gj_graph_milestone_48500(void)
    __attribute__((alias("gj_graph_milestone_48500")));
