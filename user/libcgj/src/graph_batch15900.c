/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch15900: MILESTONE 15900 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_15900(void);
 *     - Returns the compile-time graph batch number for this TU (15900).
 *   uint32_t gj_graph_milestone_15900(void);
 *     - Returns the current graph milestone revision (15900).
 *   uint32_t __gj_batch_id_15900  (alias)
 *   uint32_t __gj_graph_milestone_15900  (alias)
 *   __libcgj_batch15900_marker = "libcgj-batch15900"
 *
 * MILESTONE 15900 for the exclusive continuum CREATE-ONLY wave
 * (batches 15891-15899: shell_green_15900, libcgj_green_15900,
 * bar3_ready_15900, product_score_15900, continuum_ready_15900,
 * smoke_soft_15900, dyn_soft_15900, milestone_tag_15900,
 * continuum_wave_15900). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=15900. bar3 remains open (gj_bar3_ready_15900 == 0);
 * product_score remains open (gj_product_score_15900 == 0).
 *
 * CGJ soft marker band capstone (15891–15900): host soft probes
 * (cgj_soft_milestone_15900.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 15900. */
const char __libcgj_batch15900_marker[] = "libcgj-batch15900";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b15900_id(void)
{
	return 15900u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_15900 - report this TU's graph batch number.
 *
 * Always returns 15900u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 15900.
 */
uint32_t
gj_batch_id_15900(void)
{
	(void)NULL;
	return b15900_id();
}

/*
 * gj_graph_milestone_15900 - report this TU's graph milestone revision.
 *
 * Always returns 15900u (MILESTONE 15900). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 15900.
 */
uint32_t
gj_graph_milestone_15900(void)
{
	return b15900_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_15900(void)
    __attribute__((alias("gj_batch_id_15900")));

uint32_t __gj_graph_milestone_15900(void)
    __attribute__((alias("gj_graph_milestone_15900")));
