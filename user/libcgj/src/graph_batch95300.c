/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch95300: MILESTONE 95300 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_95300(void);
 *     - Returns the compile-time graph batch number for this TU (95300).
 *   uint32_t gj_graph_milestone_95300(void);
 *     - Returns the current graph milestone revision (95300).
 *   uint32_t __gj_batch_id_95300  (alias)
 *   uint32_t __gj_graph_milestone_95300  (alias)
 *   __libcgj_batch95300_marker = "libcgj-batch95300"
 *
 * MILESTONE 95300 for the exclusive continuum CREATE-ONLY wave
 * (batches 25891-25899: shell_green_95300, libcgj_green_95300,
 * bar3_ready_95300, product_score_95300, continuum_ready_95300,
 * smoke_soft_95300, dyn_soft_95300, milestone_tag_95300,
 * continuum_wave_95300). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=95300. bar3 remains open (gj_bar3_ready_95300 == 0);
 * product_score remains open (gj_product_score_95300 == 0).
 *
 * CGJ soft marker band capstone (25891–95300): host soft probes
 * (cgj_soft_milestone_95300.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 95300. */
const char __libcgj_batch95300_marker[] = "libcgj-batch95300";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b95300_id(void)
{
	return 95300u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_95300 - report this TU's graph batch number.
 *
 * Always returns 95300u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 95300.
 */
uint32_t
gj_batch_id_95300(void)
{
	(void)NULL;
	return b95300_id();
}

/*
 * gj_graph_milestone_95300 - report this TU's graph milestone revision.
 *
 * Always returns 95300u (MILESTONE 95300). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 95300.
 */
uint32_t
gj_graph_milestone_95300(void)
{
	return b95300_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_95300(void)
    __attribute__((alias("gj_batch_id_95300")));

uint32_t __gj_graph_milestone_95300(void)
    __attribute__((alias("gj_graph_milestone_95300")));
