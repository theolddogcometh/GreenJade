/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch60400: MILESTONE 60400 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_60400(void);
 *     - Returns the compile-time graph batch number for this TU (60400).
 *   uint32_t gj_graph_milestone_60400(void);
 *     - Returns the current graph milestone revision (60400).
 *   uint32_t __gj_batch_id_60400  (alias)
 *   uint32_t __gj_graph_milestone_60400  (alias)
 *   __libcgj_batch60400_marker = "libcgj-batch60400"
 *
 * MILESTONE 60400 for the exclusive continuum CREATE-ONLY wave
 * (batches 25891-25899: shell_green_60400, libcgj_green_60400,
 * bar3_ready_60400, product_score_60400, continuum_ready_60400,
 * smoke_soft_60400, dyn_soft_60400, milestone_tag_60400,
 * continuum_wave_60400). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=60400. bar3 remains open (gj_bar3_ready_60400 == 0);
 * product_score remains open (gj_product_score_60400 == 0).
 *
 * CGJ soft marker band capstone (25891–60400): host soft probes
 * (cgj_soft_milestone_60400.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 60400. */
const char __libcgj_batch60400_marker[] = "libcgj-batch60400";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b60400_id(void)
{
	return 60400u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_60400 - report this TU's graph batch number.
 *
 * Always returns 60400u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 60400.
 */
uint32_t
gj_batch_id_60400(void)
{
	(void)NULL;
	return b60400_id();
}

/*
 * gj_graph_milestone_60400 - report this TU's graph milestone revision.
 *
 * Always returns 60400u (MILESTONE 60400). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 60400.
 */
uint32_t
gj_graph_milestone_60400(void)
{
	return b60400_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_60400(void)
    __attribute__((alias("gj_batch_id_60400")));

uint32_t __gj_graph_milestone_60400(void)
    __attribute__((alias("gj_graph_milestone_60400")));
