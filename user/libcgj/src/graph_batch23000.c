/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch23000: MILESTONE 23000 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_23000(void);
 *     - Returns the compile-time graph batch number for this TU (23000).
 *   uint32_t gj_graph_milestone_23000(void);
 *     - Returns the current graph milestone revision (23000).
 *   uint32_t __gj_batch_id_23000  (alias)
 *   uint32_t __gj_graph_milestone_23000  (alias)
 *   __libcgj_batch23000_marker = "libcgj-batch23000"
 *
 * MILESTONE 23000 for the exclusive continuum CREATE-ONLY wave
 * (batches 22991-22999: shell_green_23000, libcgj_green_23000,
 * bar3_ready_23000, product_score_23000, continuum_ready_23000,
 * smoke_soft_23000, dyn_soft_23000, milestone_tag_23000,
 * continuum_wave_23000). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=23000. bar3 remains open (gj_bar3_ready_23000 == 0);
 * product_score remains open (gj_product_score_23000 == 0).
 *
 * CGJ soft marker band capstone (22991–23000): host soft probes
 * (cgj_soft_milestone_23000.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 23000. */
const char __libcgj_batch23000_marker[] = "libcgj-batch23000";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b23000_id(void)
{
	return 23000u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_23000 - report this TU's graph batch number.
 *
 * Always returns 23000u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 23000.
 */
uint32_t
gj_batch_id_23000(void)
{
	(void)NULL;
	return b23000_id();
}

/*
 * gj_graph_milestone_23000 - report this TU's graph milestone revision.
 *
 * Always returns 23000u (MILESTONE 23000). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 23000.
 */
uint32_t
gj_graph_milestone_23000(void)
{
	return b23000_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_23000(void)
    __attribute__((alias("gj_batch_id_23000")));

uint32_t __gj_graph_milestone_23000(void)
    __attribute__((alias("gj_graph_milestone_23000")));
