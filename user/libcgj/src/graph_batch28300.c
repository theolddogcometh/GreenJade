/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch28300: MILESTONE 28300 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_28300(void);
 *     - Returns the compile-time graph batch number for this TU (28300).
 *   uint32_t gj_graph_milestone_28300(void);
 *     - Returns the current graph milestone revision (28300).
 *   uint32_t __gj_batch_id_28300  (alias)
 *   uint32_t __gj_graph_milestone_28300  (alias)
 *   __libcgj_batch28300_marker = "libcgj-batch28300"
 *
 * MILESTONE 28300 for the exclusive continuum CREATE-ONLY wave
 * (batches 28291-28299: shell_green_28300, libcgj_green_28300,
 * bar3_ready_28300, product_score_28300, continuum_ready_28300,
 * smoke_soft_28300, dyn_soft_28300, milestone_tag_28300,
 * continuum_wave_28300). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=28300. bar3 remains open (gj_bar3_ready_28300 == 0);
 * product_score remains open (gj_product_score_28300 == 0).
 *
 * CGJ soft marker band capstone (28291–28300): host soft probes
 * (cgj_soft_milestone_28300.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 28300. */
const char __libcgj_batch28300_marker[] = "libcgj-batch28300";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b28300_id(void)
{
	return 28300u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_28300 - report this TU's graph batch number.
 *
 * Always returns 28300u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 28300.
 */
uint32_t
gj_batch_id_28300(void)
{
	(void)NULL;
	return b28300_id();
}

/*
 * gj_graph_milestone_28300 - report this TU's graph milestone revision.
 *
 * Always returns 28300u (MILESTONE 28300). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 28300.
 */
uint32_t
gj_graph_milestone_28300(void)
{
	return b28300_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_28300(void)
    __attribute__((alias("gj_batch_id_28300")));

uint32_t __gj_graph_milestone_28300(void)
    __attribute__((alias("gj_graph_milestone_28300")));
