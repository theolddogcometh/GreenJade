/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch28800: MILESTONE 28800 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_28800(void);
 *     - Returns the compile-time graph batch number for this TU (28800).
 *   uint32_t gj_graph_milestone_28800(void);
 *     - Returns the current graph milestone revision (28800).
 *   uint32_t __gj_batch_id_28800  (alias)
 *   uint32_t __gj_graph_milestone_28800  (alias)
 *   __libcgj_batch28800_marker = "libcgj-batch28800"
 *
 * MILESTONE 28800 for the exclusive continuum CREATE-ONLY wave
 * (batches 28791-28799: shell_green_28800, libcgj_green_28800,
 * bar3_ready_28800, product_score_28800, continuum_ready_28800,
 * smoke_soft_28800, dyn_soft_28800, milestone_tag_28800,
 * continuum_wave_28800). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=28800. bar3 remains open (gj_bar3_ready_28800 == 0);
 * product_score remains open (gj_product_score_28800 == 0).
 *
 * CGJ soft marker band capstone (28791–28800): host soft probes
 * (cgj_soft_milestone_28800.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 28800. */
const char __libcgj_batch28800_marker[] = "libcgj-batch28800";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b28800_id(void)
{
	return 28800u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_28800 - report this TU's graph batch number.
 *
 * Always returns 28800u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 28800.
 */
uint32_t
gj_batch_id_28800(void)
{
	(void)NULL;
	return b28800_id();
}

/*
 * gj_graph_milestone_28800 - report this TU's graph milestone revision.
 *
 * Always returns 28800u (MILESTONE 28800). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 28800.
 */
uint32_t
gj_graph_milestone_28800(void)
{
	return b28800_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_28800(void)
    __attribute__((alias("gj_batch_id_28800")));

uint32_t __gj_graph_milestone_28800(void)
    __attribute__((alias("gj_graph_milestone_28800")));
