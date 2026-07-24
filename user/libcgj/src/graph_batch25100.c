/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch25100: MILESTONE 25100 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_25100(void);
 *     - Returns the compile-time graph batch number for this TU (25100).
 *   uint32_t gj_graph_milestone_25100(void);
 *     - Returns the current graph milestone revision (25100).
 *   uint32_t __gj_batch_id_25100  (alias)
 *   uint32_t __gj_graph_milestone_25100  (alias)
 *   __libcgj_batch25100_marker = "libcgj-batch25100"
 *
 * MILESTONE 25100 for the exclusive continuum CREATE-ONLY wave
 * (batches 25091-25099: shell_green_25100, libcgj_green_25100,
 * bar3_ready_25100, product_score_25100, continuum_ready_25100,
 * smoke_soft_25100, dyn_soft_25100, milestone_tag_25100,
 * continuum_wave_25100). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=25100. bar3 remains open (gj_bar3_ready_25100 == 0);
 * product_score remains open (gj_product_score_25100 == 0).
 *
 * CGJ soft marker band capstone (25091–25100): host soft probes
 * (cgj_soft_milestone_25100.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 25100. */
const char __libcgj_batch25100_marker[] = "libcgj-batch25100";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b25100_id(void)
{
	return 25100u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_25100 - report this TU's graph batch number.
 *
 * Always returns 25100u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 25100.
 */
uint32_t
gj_batch_id_25100(void)
{
	(void)NULL;
	return b25100_id();
}

/*
 * gj_graph_milestone_25100 - report this TU's graph milestone revision.
 *
 * Always returns 25100u (MILESTONE 25100). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 25100.
 */
uint32_t
gj_graph_milestone_25100(void)
{
	return b25100_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_25100(void)
    __attribute__((alias("gj_batch_id_25100")));

uint32_t __gj_graph_milestone_25100(void)
    __attribute__((alias("gj_graph_milestone_25100")));
