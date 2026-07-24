/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch48400: MILESTONE 48400 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_48400(void);
 *     - Returns the compile-time graph batch number for this TU (48400).
 *   uint32_t gj_graph_milestone_48400(void);
 *     - Returns the current graph milestone revision (48400).
 *   uint32_t __gj_batch_id_48400  (alias)
 *   uint32_t __gj_graph_milestone_48400  (alias)
 *   __libcgj_batch48400_marker = "libcgj-batch48400"
 *
 * MILESTONE 48400 for the exclusive continuum CREATE-ONLY wave
 * (batches 48391-48399: shell_green_48400, libcgj_green_48400,
 * bar3_ready_48400, product_score_48400, continuum_ready_48400,
 * smoke_soft_48400, dyn_soft_48400, milestone_tag_48400,
 * continuum_wave_48400). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=48400. bar3 remains open (gj_bar3_ready_48400 == 0);
 * product_score remains open (gj_product_score_48400 == 0).
 *
 * CGJ soft marker band capstone (48391–48400): host soft probes
 * (cgj_soft_milestone_48400.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 48400. */
const char __libcgj_batch48400_marker[] = "libcgj-batch48400";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b48400_id(void)
{
	return 48400u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_48400 - report this TU's graph batch number.
 *
 * Always returns 48400u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 48400.
 */
uint32_t
gj_batch_id_48400(void)
{
	(void)NULL;
	return b48400_id();
}

/*
 * gj_graph_milestone_48400 - report this TU's graph milestone revision.
 *
 * Always returns 48400u (MILESTONE 48400). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 48400.
 */
uint32_t
gj_graph_milestone_48400(void)
{
	return b48400_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_48400(void)
    __attribute__((alias("gj_batch_id_48400")));

uint32_t __gj_graph_milestone_48400(void)
    __attribute__((alias("gj_graph_milestone_48400")));
