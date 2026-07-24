/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch40800: MILESTONE 40800 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_40800(void);
 *     - Returns the compile-time graph batch number for this TU (40800).
 *   uint32_t gj_graph_milestone_40800(void);
 *     - Returns the current graph milestone revision (40800).
 *   uint32_t __gj_batch_id_40800  (alias)
 *   uint32_t __gj_graph_milestone_40800  (alias)
 *   __libcgj_batch40800_marker = "libcgj-batch40800"
 *
 * MILESTONE 40800 for the exclusive continuum CREATE-ONLY wave
 * (batches 40791-40799: shell_green_40800, libcgj_green_40800,
 * bar3_ready_40800, product_score_40800, continuum_ready_40800,
 * smoke_soft_40800, dyn_soft_40800, milestone_tag_40800,
 * continuum_wave_40800). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=40800. bar3 remains open (gj_bar3_ready_40800 == 0);
 * product_score remains open (gj_product_score_40800 == 0).
 *
 * CGJ soft marker band capstone (40791–40800): host soft probes
 * (cgj_soft_milestone_40800.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 40800. */
const char __libcgj_batch40800_marker[] = "libcgj-batch40800";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b40800_id(void)
{
	return 40800u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_40800 - report this TU's graph batch number.
 *
 * Always returns 40800u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 40800.
 */
uint32_t
gj_batch_id_40800(void)
{
	(void)NULL;
	return b40800_id();
}

/*
 * gj_graph_milestone_40800 - report this TU's graph milestone revision.
 *
 * Always returns 40800u (MILESTONE 40800). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 40800.
 */
uint32_t
gj_graph_milestone_40800(void)
{
	return b40800_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_40800(void)
    __attribute__((alias("gj_batch_id_40800")));

uint32_t __gj_graph_milestone_40800(void)
    __attribute__((alias("gj_graph_milestone_40800")));
