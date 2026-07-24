/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch55300: MILESTONE 55300 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_55300(void);
 *     - Returns the compile-time graph batch number for this TU (55300).
 *   uint32_t gj_graph_milestone_55300(void);
 *     - Returns the current graph milestone revision (55300).
 *   uint32_t __gj_batch_id_55300  (alias)
 *   uint32_t __gj_graph_milestone_55300  (alias)
 *   __libcgj_batch55300_marker = "libcgj-batch55300"
 *
 * MILESTONE 55300 for the exclusive continuum CREATE-ONLY wave
 * (batches 25891-25899: shell_green_55300, libcgj_green_55300,
 * bar3_ready_55300, product_score_55300, continuum_ready_55300,
 * smoke_soft_55300, dyn_soft_55300, milestone_tag_55300,
 * continuum_wave_55300). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=55300. bar3 remains open (gj_bar3_ready_55300 == 0);
 * product_score remains open (gj_product_score_55300 == 0).
 *
 * CGJ soft marker band capstone (25891–55300): host soft probes
 * (cgj_soft_milestone_55300.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 55300. */
const char __libcgj_batch55300_marker[] = "libcgj-batch55300";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b55300_id(void)
{
	return 55300u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_55300 - report this TU's graph batch number.
 *
 * Always returns 55300u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 55300.
 */
uint32_t
gj_batch_id_55300(void)
{
	(void)NULL;
	return b55300_id();
}

/*
 * gj_graph_milestone_55300 - report this TU's graph milestone revision.
 *
 * Always returns 55300u (MILESTONE 55300). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 55300.
 */
uint32_t
gj_graph_milestone_55300(void)
{
	return b55300_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_55300(void)
    __attribute__((alias("gj_batch_id_55300")));

uint32_t __gj_graph_milestone_55300(void)
    __attribute__((alias("gj_graph_milestone_55300")));
