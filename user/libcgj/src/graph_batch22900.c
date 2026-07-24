/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch22900: MILESTONE 22900 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_22900(void);
 *     - Returns the compile-time graph batch number for this TU (22900).
 *   uint32_t gj_graph_milestone_22900(void);
 *     - Returns the current graph milestone revision (22900).
 *   uint32_t __gj_batch_id_22900  (alias)
 *   uint32_t __gj_graph_milestone_22900  (alias)
 *   __libcgj_batch22900_marker = "libcgj-batch22900"
 *
 * MILESTONE 22900 for the exclusive continuum CREATE-ONLY wave
 * (batches 22891-22899: shell_green_22900, libcgj_green_22900,
 * bar3_ready_22900, product_score_22900, continuum_ready_22900,
 * smoke_soft_22900, dyn_soft_22900, milestone_tag_22900,
 * continuum_wave_22900). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=22900. bar3 remains open (gj_bar3_ready_22900 == 0);
 * product_score remains open (gj_product_score_22900 == 0).
 *
 * CGJ soft marker band capstone (22891–22900): host soft probes
 * (cgj_soft_milestone_22900.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 22900. */
const char __libcgj_batch22900_marker[] = "libcgj-batch22900";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b22900_id(void)
{
	return 22900u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_22900 - report this TU's graph batch number.
 *
 * Always returns 22900u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 22900.
 */
uint32_t
gj_batch_id_22900(void)
{
	(void)NULL;
	return b22900_id();
}

/*
 * gj_graph_milestone_22900 - report this TU's graph milestone revision.
 *
 * Always returns 22900u (MILESTONE 22900). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 22900.
 */
uint32_t
gj_graph_milestone_22900(void)
{
	return b22900_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_22900(void)
    __attribute__((alias("gj_batch_id_22900")));

uint32_t __gj_graph_milestone_22900(void)
    __attribute__((alias("gj_graph_milestone_22900")));
