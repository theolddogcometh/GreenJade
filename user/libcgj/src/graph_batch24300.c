/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch24300: MILESTONE 24300 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_24300(void);
 *     - Returns the compile-time graph batch number for this TU (24300).
 *   uint32_t gj_graph_milestone_24300(void);
 *     - Returns the current graph milestone revision (24300).
 *   uint32_t __gj_batch_id_24300  (alias)
 *   uint32_t __gj_graph_milestone_24300  (alias)
 *   __libcgj_batch24300_marker = "libcgj-batch24300"
 *
 * MILESTONE 24300 for the exclusive continuum CREATE-ONLY wave
 * (batches 24291-24299: shell_green_24300, libcgj_green_24300,
 * bar3_ready_24300, product_score_24300, continuum_ready_24300,
 * smoke_soft_24300, dyn_soft_24300, milestone_tag_24300,
 * continuum_wave_24300). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=24300. bar3 remains open (gj_bar3_ready_24300 == 0);
 * product_score remains open (gj_product_score_24300 == 0).
 *
 * CGJ soft marker band capstone (24291–24300): host soft probes
 * (cgj_soft_milestone_24300.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 24300. */
const char __libcgj_batch24300_marker[] = "libcgj-batch24300";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b24300_id(void)
{
	return 24300u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_24300 - report this TU's graph batch number.
 *
 * Always returns 24300u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 24300.
 */
uint32_t
gj_batch_id_24300(void)
{
	(void)NULL;
	return b24300_id();
}

/*
 * gj_graph_milestone_24300 - report this TU's graph milestone revision.
 *
 * Always returns 24300u (MILESTONE 24300). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 24300.
 */
uint32_t
gj_graph_milestone_24300(void)
{
	return b24300_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_24300(void)
    __attribute__((alias("gj_batch_id_24300")));

uint32_t __gj_graph_milestone_24300(void)
    __attribute__((alias("gj_graph_milestone_24300")));
