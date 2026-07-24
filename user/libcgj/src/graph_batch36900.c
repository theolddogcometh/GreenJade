/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch36900: MILESTONE 36900 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_36900(void);
 *     - Returns the compile-time graph batch number for this TU (36900).
 *   uint32_t gj_graph_milestone_36900(void);
 *     - Returns the current graph milestone revision (36900).
 *   uint32_t __gj_batch_id_36900  (alias)
 *   uint32_t __gj_graph_milestone_36900  (alias)
 *   __libcgj_batch36900_marker = "libcgj-batch36900"
 *
 * MILESTONE 36900 for the exclusive continuum CREATE-ONLY wave
 * (batches 36891-36899: shell_green_36900, libcgj_green_36900,
 * bar3_ready_36900, product_score_36900, continuum_ready_36900,
 * smoke_soft_36900, dyn_soft_36900, milestone_tag_36900,
 * continuum_wave_36900). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=36900. bar3 remains open (gj_bar3_ready_36900 == 0);
 * product_score remains open (gj_product_score_36900 == 0).
 *
 * CGJ soft marker band capstone (36891–36900): host soft probes
 * (cgj_soft_milestone_36900.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 36900. */
const char __libcgj_batch36900_marker[] = "libcgj-batch36900";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b36900_id(void)
{
	return 36900u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_36900 - report this TU's graph batch number.
 *
 * Always returns 36900u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 36900.
 */
uint32_t
gj_batch_id_36900(void)
{
	(void)NULL;
	return b36900_id();
}

/*
 * gj_graph_milestone_36900 - report this TU's graph milestone revision.
 *
 * Always returns 36900u (MILESTONE 36900). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 36900.
 */
uint32_t
gj_graph_milestone_36900(void)
{
	return b36900_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_36900(void)
    __attribute__((alias("gj_batch_id_36900")));

uint32_t __gj_graph_milestone_36900(void)
    __attribute__((alias("gj_graph_milestone_36900")));
