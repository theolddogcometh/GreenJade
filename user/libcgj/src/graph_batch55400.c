/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch55400: MILESTONE 55400 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_55400(void);
 *     - Returns the compile-time graph batch number for this TU (55400).
 *   uint32_t gj_graph_milestone_55400(void);
 *     - Returns the current graph milestone revision (55400).
 *   uint32_t __gj_batch_id_55400  (alias)
 *   uint32_t __gj_graph_milestone_55400  (alias)
 *   __libcgj_batch55400_marker = "libcgj-batch55400"
 *
 * MILESTONE 55400 for the exclusive continuum CREATE-ONLY wave
 * (batches 25891-25899: shell_green_55400, libcgj_green_55400,
 * bar3_ready_55400, product_score_55400, continuum_ready_55400,
 * smoke_soft_55400, dyn_soft_55400, milestone_tag_55400,
 * continuum_wave_55400). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=55400. bar3 remains open (gj_bar3_ready_55400 == 0);
 * product_score remains open (gj_product_score_55400 == 0).
 *
 * CGJ soft marker band capstone (25891–55400): host soft probes
 * (cgj_soft_milestone_55400.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 55400. */
const char __libcgj_batch55400_marker[] = "libcgj-batch55400";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b55400_id(void)
{
	return 55400u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_55400 - report this TU's graph batch number.
 *
 * Always returns 55400u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 55400.
 */
uint32_t
gj_batch_id_55400(void)
{
	(void)NULL;
	return b55400_id();
}

/*
 * gj_graph_milestone_55400 - report this TU's graph milestone revision.
 *
 * Always returns 55400u (MILESTONE 55400). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 55400.
 */
uint32_t
gj_graph_milestone_55400(void)
{
	return b55400_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_55400(void)
    __attribute__((alias("gj_batch_id_55400")));

uint32_t __gj_graph_milestone_55400(void)
    __attribute__((alias("gj_graph_milestone_55400")));
