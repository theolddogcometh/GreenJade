/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch30900: MILESTONE 30900 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_30900(void);
 *     - Returns the compile-time graph batch number for this TU (30900).
 *   uint32_t gj_graph_milestone_30900(void);
 *     - Returns the current graph milestone revision (30900).
 *   uint32_t __gj_batch_id_30900  (alias)
 *   uint32_t __gj_graph_milestone_30900  (alias)
 *   __libcgj_batch30900_marker = "libcgj-batch30900"
 *
 * MILESTONE 30900 for the exclusive continuum CREATE-ONLY wave
 * (batches 30891-30899: shell_green_30900, libcgj_green_30900,
 * bar3_ready_30900, product_score_30900, continuum_ready_30900,
 * smoke_soft_30900, dyn_soft_30900, milestone_tag_30900,
 * continuum_wave_30900). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=30900. bar3 remains open (gj_bar3_ready_30900 == 0);
 * product_score remains open (gj_product_score_30900 == 0).
 *
 * CGJ soft marker band capstone (30891–30900): host soft probes
 * (cgj_soft_milestone_30900.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 30900. */
const char __libcgj_batch30900_marker[] = "libcgj-batch30900";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b30900_id(void)
{
	return 30900u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_30900 - report this TU's graph batch number.
 *
 * Always returns 30900u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 30900.
 */
uint32_t
gj_batch_id_30900(void)
{
	(void)NULL;
	return b30900_id();
}

/*
 * gj_graph_milestone_30900 - report this TU's graph milestone revision.
 *
 * Always returns 30900u (MILESTONE 30900). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 30900.
 */
uint32_t
gj_graph_milestone_30900(void)
{
	return b30900_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_30900(void)
    __attribute__((alias("gj_batch_id_30900")));

uint32_t __gj_graph_milestone_30900(void)
    __attribute__((alias("gj_graph_milestone_30900")));
