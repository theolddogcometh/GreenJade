/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch90300: MILESTONE 90300 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_90300(void);
 *     - Returns the compile-time graph batch number for this TU (90300).
 *   uint32_t gj_graph_milestone_90300(void);
 *     - Returns the current graph milestone revision (90300).
 *   uint32_t __gj_batch_id_90300  (alias)
 *   uint32_t __gj_graph_milestone_90300  (alias)
 *   __libcgj_batch90300_marker = "libcgj-batch90300"
 *
 * MILESTONE 90300 for the exclusive continuum CREATE-ONLY wave
 * (batches 25891-25899: shell_green_90300, libcgj_green_90300,
 * bar3_ready_90300, product_score_90300, continuum_ready_90300,
 * smoke_soft_90300, dyn_soft_90300, milestone_tag_90300,
 * continuum_wave_90300). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=90300. bar3 remains open (gj_bar3_ready_90300 == 0);
 * product_score remains open (gj_product_score_90300 == 0).
 *
 * CGJ soft marker band capstone (25891–90300): host soft probes
 * (cgj_soft_milestone_90300.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 90300. */
const char __libcgj_batch90300_marker[] = "libcgj-batch90300";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b90300_id(void)
{
	return 90300u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_90300 - report this TU's graph batch number.
 *
 * Always returns 90300u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 90300.
 */
uint32_t
gj_batch_id_90300(void)
{
	(void)NULL;
	return b90300_id();
}

/*
 * gj_graph_milestone_90300 - report this TU's graph milestone revision.
 *
 * Always returns 90300u (MILESTONE 90300). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 90300.
 */
uint32_t
gj_graph_milestone_90300(void)
{
	return b90300_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_90300(void)
    __attribute__((alias("gj_batch_id_90300")));

uint32_t __gj_graph_milestone_90300(void)
    __attribute__((alias("gj_graph_milestone_90300")));
