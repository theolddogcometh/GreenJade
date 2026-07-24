/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch65400: MILESTONE 65400 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_65400(void);
 *     - Returns the compile-time graph batch number for this TU (65400).
 *   uint32_t gj_graph_milestone_65400(void);
 *     - Returns the current graph milestone revision (65400).
 *   uint32_t __gj_batch_id_65400  (alias)
 *   uint32_t __gj_graph_milestone_65400  (alias)
 *   __libcgj_batch65400_marker = "libcgj-batch65400"
 *
 * MILESTONE 65400 for the exclusive continuum CREATE-ONLY wave
 * (batches 25891-25899: shell_green_65400, libcgj_green_65400,
 * bar3_ready_65400, product_score_65400, continuum_ready_65400,
 * smoke_soft_65400, dyn_soft_65400, milestone_tag_65400,
 * continuum_wave_65400). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=65400. bar3 remains open (gj_bar3_ready_65400 == 0);
 * product_score remains open (gj_product_score_65400 == 0).
 *
 * CGJ soft marker band capstone (25891–65400): host soft probes
 * (cgj_soft_milestone_65400.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 65400. */
const char __libcgj_batch65400_marker[] = "libcgj-batch65400";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b65400_id(void)
{
	return 65400u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_65400 - report this TU's graph batch number.
 *
 * Always returns 65400u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 65400.
 */
uint32_t
gj_batch_id_65400(void)
{
	(void)NULL;
	return b65400_id();
}

/*
 * gj_graph_milestone_65400 - report this TU's graph milestone revision.
 *
 * Always returns 65400u (MILESTONE 65400). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 65400.
 */
uint32_t
gj_graph_milestone_65400(void)
{
	return b65400_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_65400(void)
    __attribute__((alias("gj_batch_id_65400")));

uint32_t __gj_graph_milestone_65400(void)
    __attribute__((alias("gj_graph_milestone_65400")));
