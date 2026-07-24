/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch56300: MILESTONE 56300 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_56300(void);
 *     - Returns the compile-time graph batch number for this TU (56300).
 *   uint32_t gj_graph_milestone_56300(void);
 *     - Returns the current graph milestone revision (56300).
 *   uint32_t __gj_batch_id_56300  (alias)
 *   uint32_t __gj_graph_milestone_56300  (alias)
 *   __libcgj_batch56300_marker = "libcgj-batch56300"
 *
 * MILESTONE 56300 for the exclusive continuum CREATE-ONLY wave
 * (batches 25891-25899: shell_green_56300, libcgj_green_56300,
 * bar3_ready_56300, product_score_56300, continuum_ready_56300,
 * smoke_soft_56300, dyn_soft_56300, milestone_tag_56300,
 * continuum_wave_56300). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=56300. bar3 remains open (gj_bar3_ready_56300 == 0);
 * product_score remains open (gj_product_score_56300 == 0).
 *
 * CGJ soft marker band capstone (25891–56300): host soft probes
 * (cgj_soft_milestone_56300.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 56300. */
const char __libcgj_batch56300_marker[] = "libcgj-batch56300";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b56300_id(void)
{
	return 56300u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_56300 - report this TU's graph batch number.
 *
 * Always returns 56300u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 56300.
 */
uint32_t
gj_batch_id_56300(void)
{
	(void)NULL;
	return b56300_id();
}

/*
 * gj_graph_milestone_56300 - report this TU's graph milestone revision.
 *
 * Always returns 56300u (MILESTONE 56300). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 56300.
 */
uint32_t
gj_graph_milestone_56300(void)
{
	return b56300_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_56300(void)
    __attribute__((alias("gj_batch_id_56300")));

uint32_t __gj_graph_milestone_56300(void)
    __attribute__((alias("gj_graph_milestone_56300")));
