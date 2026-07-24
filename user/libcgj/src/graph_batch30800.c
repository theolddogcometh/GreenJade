/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch30800: MILESTONE 30800 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_30800(void);
 *     - Returns the compile-time graph batch number for this TU (30800).
 *   uint32_t gj_graph_milestone_30800(void);
 *     - Returns the current graph milestone revision (30800).
 *   uint32_t __gj_batch_id_30800  (alias)
 *   uint32_t __gj_graph_milestone_30800  (alias)
 *   __libcgj_batch30800_marker = "libcgj-batch30800"
 *
 * MILESTONE 30800 for the exclusive continuum CREATE-ONLY wave
 * (batches 30791-30799: shell_green_30800, libcgj_green_30800,
 * bar3_ready_30800, product_score_30800, continuum_ready_30800,
 * smoke_soft_30800, dyn_soft_30800, milestone_tag_30800,
 * continuum_wave_30800). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=30800. bar3 remains open (gj_bar3_ready_30800 == 0);
 * product_score remains open (gj_product_score_30800 == 0).
 *
 * CGJ soft marker band capstone (30791–30800): host soft probes
 * (cgj_soft_milestone_30800.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 30800. */
const char __libcgj_batch30800_marker[] = "libcgj-batch30800";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b30800_id(void)
{
	return 30800u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_30800 - report this TU's graph batch number.
 *
 * Always returns 30800u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 30800.
 */
uint32_t
gj_batch_id_30800(void)
{
	(void)NULL;
	return b30800_id();
}

/*
 * gj_graph_milestone_30800 - report this TU's graph milestone revision.
 *
 * Always returns 30800u (MILESTONE 30800). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 30800.
 */
uint32_t
gj_graph_milestone_30800(void)
{
	return b30800_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_30800(void)
    __attribute__((alias("gj_batch_id_30800")));

uint32_t __gj_graph_milestone_30800(void)
    __attribute__((alias("gj_graph_milestone_30800")));
