/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch30300: MILESTONE 30300 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_30300(void);
 *     - Returns the compile-time graph batch number for this TU (30300).
 *   uint32_t gj_graph_milestone_30300(void);
 *     - Returns the current graph milestone revision (30300).
 *   uint32_t __gj_batch_id_30300  (alias)
 *   uint32_t __gj_graph_milestone_30300  (alias)
 *   __libcgj_batch30300_marker = "libcgj-batch30300"
 *
 * MILESTONE 30300 for the exclusive continuum CREATE-ONLY wave
 * (batches 30291-30299: shell_green_30300, libcgj_green_30300,
 * bar3_ready_30300, product_score_30300, continuum_ready_30300,
 * smoke_soft_30300, dyn_soft_30300, milestone_tag_30300,
 * continuum_wave_30300). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=30300. bar3 remains open (gj_bar3_ready_30300 == 0);
 * product_score remains open (gj_product_score_30300 == 0).
 *
 * CGJ soft marker band capstone (30291–30300): host soft probes
 * (cgj_soft_milestone_30300.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 30300. */
const char __libcgj_batch30300_marker[] = "libcgj-batch30300";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b30300_id(void)
{
	return 30300u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_30300 - report this TU's graph batch number.
 *
 * Always returns 30300u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 30300.
 */
uint32_t
gj_batch_id_30300(void)
{
	(void)NULL;
	return b30300_id();
}

/*
 * gj_graph_milestone_30300 - report this TU's graph milestone revision.
 *
 * Always returns 30300u (MILESTONE 30300). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 30300.
 */
uint32_t
gj_graph_milestone_30300(void)
{
	return b30300_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_30300(void)
    __attribute__((alias("gj_batch_id_30300")));

uint32_t __gj_graph_milestone_30300(void)
    __attribute__((alias("gj_graph_milestone_30300")));
