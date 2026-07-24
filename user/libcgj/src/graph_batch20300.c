/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch20300: MILESTONE 20300 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_20300(void);
 *     - Returns the compile-time graph batch number for this TU (20300).
 *   uint32_t gj_graph_milestone_20300(void);
 *     - Returns the current graph milestone revision (20300).
 *   uint32_t __gj_batch_id_20300  (alias)
 *   uint32_t __gj_graph_milestone_20300  (alias)
 *   __libcgj_batch20300_marker = "libcgj-batch20300"
 *
 * MILESTONE 20300 for the exclusive continuum CREATE-ONLY wave
 * (batches 20291-20299: shell_green_20300, libcgj_green_20300,
 * bar3_ready_20300, product_score_20300, continuum_ready_20300,
 * smoke_soft_20300, dyn_soft_20300, milestone_tag_20300,
 * continuum_wave_20300). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=20300. bar3 remains open (gj_bar3_ready_20300 == 0);
 * product_score remains open (gj_product_score_20300 == 0).
 *
 * CGJ soft marker band capstone (20291–20300): host soft probes
 * (cgj_soft_milestone_20300.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 20300. */
const char __libcgj_batch20300_marker[] = "libcgj-batch20300";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b20300_id(void)
{
	return 20300u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_20300 - report this TU's graph batch number.
 *
 * Always returns 20300u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 20300.
 */
uint32_t
gj_batch_id_20300(void)
{
	(void)NULL;
	return b20300_id();
}

/*
 * gj_graph_milestone_20300 - report this TU's graph milestone revision.
 *
 * Always returns 20300u (MILESTONE 20300). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 20300.
 */
uint32_t
gj_graph_milestone_20300(void)
{
	return b20300_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_20300(void)
    __attribute__((alias("gj_batch_id_20300")));

uint32_t __gj_graph_milestone_20300(void)
    __attribute__((alias("gj_graph_milestone_20300")));
