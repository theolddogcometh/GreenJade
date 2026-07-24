/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch44100: MILESTONE 44100 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_44100(void);
 *     - Returns the compile-time graph batch number for this TU (44100).
 *   uint32_t gj_graph_milestone_44100(void);
 *     - Returns the current graph milestone revision (44100).
 *   uint32_t __gj_batch_id_44100  (alias)
 *   uint32_t __gj_graph_milestone_44100  (alias)
 *   __libcgj_batch44100_marker = "libcgj-batch44100"
 *
 * MILESTONE 44100 for the exclusive continuum CREATE-ONLY wave
 * (batches 44091-44099: shell_green_44100, libcgj_green_44100,
 * bar3_ready_44100, product_score_44100, continuum_ready_44100,
 * smoke_soft_44100, dyn_soft_44100, milestone_tag_44100,
 * continuum_wave_44100). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=44100. bar3 remains open (gj_bar3_ready_44100 == 0);
 * product_score remains open (gj_product_score_44100 == 0).
 *
 * CGJ soft marker band capstone (44091–44100): host soft probes
 * (cgj_soft_milestone_44100.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 44100. */
const char __libcgj_batch44100_marker[] = "libcgj-batch44100";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b44100_id(void)
{
	return 44100u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_44100 - report this TU's graph batch number.
 *
 * Always returns 44100u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 44100.
 */
uint32_t
gj_batch_id_44100(void)
{
	(void)NULL;
	return b44100_id();
}

/*
 * gj_graph_milestone_44100 - report this TU's graph milestone revision.
 *
 * Always returns 44100u (MILESTONE 44100). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 44100.
 */
uint32_t
gj_graph_milestone_44100(void)
{
	return b44100_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_44100(void)
    __attribute__((alias("gj_batch_id_44100")));

uint32_t __gj_graph_milestone_44100(void)
    __attribute__((alias("gj_graph_milestone_44100")));
