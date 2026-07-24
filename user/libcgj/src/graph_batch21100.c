/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch21100: MILESTONE 21100 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_21100(void);
 *     - Returns the compile-time graph batch number for this TU (21100).
 *   uint32_t gj_graph_milestone_21100(void);
 *     - Returns the current graph milestone revision (21100).
 *   uint32_t __gj_batch_id_21100  (alias)
 *   uint32_t __gj_graph_milestone_21100  (alias)
 *   __libcgj_batch21100_marker = "libcgj-batch21100"
 *
 * MILESTONE 21100 for the exclusive continuum CREATE-ONLY wave
 * (batches 21091-21099: shell_green_21100, libcgj_green_21100,
 * bar3_ready_21100, product_score_21100, continuum_ready_21100,
 * smoke_soft_21100, dyn_soft_21100, milestone_tag_21100,
 * continuum_wave_21100). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=21100. bar3 remains open (gj_bar3_ready_21100 == 0);
 * product_score remains open (gj_product_score_21100 == 0).
 *
 * CGJ soft marker band capstone (21091–21100): host soft probes
 * (cgj_soft_milestone_21100.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 21100. */
const char __libcgj_batch21100_marker[] = "libcgj-batch21100";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b21100_id(void)
{
	return 21100u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_21100 - report this TU's graph batch number.
 *
 * Always returns 21100u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 21100.
 */
uint32_t
gj_batch_id_21100(void)
{
	(void)NULL;
	return b21100_id();
}

/*
 * gj_graph_milestone_21100 - report this TU's graph milestone revision.
 *
 * Always returns 21100u (MILESTONE 21100). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 21100.
 */
uint32_t
gj_graph_milestone_21100(void)
{
	return b21100_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_21100(void)
    __attribute__((alias("gj_batch_id_21100")));

uint32_t __gj_graph_milestone_21100(void)
    __attribute__((alias("gj_graph_milestone_21100")));
