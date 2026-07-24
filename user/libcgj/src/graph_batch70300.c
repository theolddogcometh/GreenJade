/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch70300: MILESTONE 70300 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_70300(void);
 *     - Returns the compile-time graph batch number for this TU (70300).
 *   uint32_t gj_graph_milestone_70300(void);
 *     - Returns the current graph milestone revision (70300).
 *   uint32_t __gj_batch_id_70300  (alias)
 *   uint32_t __gj_graph_milestone_70300  (alias)
 *   __libcgj_batch70300_marker = "libcgj-batch70300"
 *
 * MILESTONE 70300 for the exclusive continuum CREATE-ONLY wave
 * (batches 25891-25899: shell_green_70300, libcgj_green_70300,
 * bar3_ready_70300, product_score_70300, continuum_ready_70300,
 * smoke_soft_70300, dyn_soft_70300, milestone_tag_70300,
 * continuum_wave_70300). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=70300. bar3 remains open (gj_bar3_ready_70300 == 0);
 * product_score remains open (gj_product_score_70300 == 0).
 *
 * CGJ soft marker band capstone (25891–70300): host soft probes
 * (cgj_soft_milestone_70300.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 70300. */
const char __libcgj_batch70300_marker[] = "libcgj-batch70300";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b70300_id(void)
{
	return 70300u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_70300 - report this TU's graph batch number.
 *
 * Always returns 70300u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 70300.
 */
uint32_t
gj_batch_id_70300(void)
{
	(void)NULL;
	return b70300_id();
}

/*
 * gj_graph_milestone_70300 - report this TU's graph milestone revision.
 *
 * Always returns 70300u (MILESTONE 70300). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 70300.
 */
uint32_t
gj_graph_milestone_70300(void)
{
	return b70300_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_70300(void)
    __attribute__((alias("gj_batch_id_70300")));

uint32_t __gj_graph_milestone_70300(void)
    __attribute__((alias("gj_graph_milestone_70300")));
