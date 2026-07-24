/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch28100: MILESTONE 28100 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_28100(void);
 *     - Returns the compile-time graph batch number for this TU (28100).
 *   uint32_t gj_graph_milestone_28100(void);
 *     - Returns the current graph milestone revision (28100).
 *   uint32_t __gj_batch_id_28100  (alias)
 *   uint32_t __gj_graph_milestone_28100  (alias)
 *   __libcgj_batch28100_marker = "libcgj-batch28100"
 *
 * MILESTONE 28100 for the exclusive continuum CREATE-ONLY wave
 * (batches 28091-28099: shell_green_28100, libcgj_green_28100,
 * bar3_ready_28100, product_score_28100, continuum_ready_28100,
 * smoke_soft_28100, dyn_soft_28100, milestone_tag_28100,
 * continuum_wave_28100). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=28100. bar3 remains open (gj_bar3_ready_28100 == 0);
 * product_score remains open (gj_product_score_28100 == 0).
 *
 * CGJ soft marker band capstone (28091–28100): host soft probes
 * (cgj_soft_milestone_28100.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 28100. */
const char __libcgj_batch28100_marker[] = "libcgj-batch28100";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b28100_id(void)
{
	return 28100u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_28100 - report this TU's graph batch number.
 *
 * Always returns 28100u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 28100.
 */
uint32_t
gj_batch_id_28100(void)
{
	(void)NULL;
	return b28100_id();
}

/*
 * gj_graph_milestone_28100 - report this TU's graph milestone revision.
 *
 * Always returns 28100u (MILESTONE 28100). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 28100.
 */
uint32_t
gj_graph_milestone_28100(void)
{
	return b28100_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_28100(void)
    __attribute__((alias("gj_batch_id_28100")));

uint32_t __gj_graph_milestone_28100(void)
    __attribute__((alias("gj_graph_milestone_28100")));
