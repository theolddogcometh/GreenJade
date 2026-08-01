/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch95600: MILESTONE 95600 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_95600(void);
 *     - Returns the compile-time graph batch number for this TU (95600).
 *   uint32_t gj_graph_milestone_95600(void);
 *     - Returns the current graph milestone revision (95600).
 *   uint32_t __gj_batch_id_95600  (alias)
 *   uint32_t __gj_graph_milestone_95600  (alias)
 *   __libcgj_batch95600_marker = "libcgj-batch95600"
 *
 * MILESTONE 95600 for the exclusive continuum CREATE-ONLY wave
 * (batches 25891-25899: shell_green_95600, libcgj_green_95600,
 * bar3_ready_95600, product_score_95600, continuum_ready_95600,
 * smoke_soft_95600, dyn_soft_95600, milestone_tag_95600,
 * continuum_wave_95600). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=95600. bar3 remains open (gj_bar3_ready_95600 == 0);
 * product_score remains open (gj_product_score_95600 == 0).
 *
 * CGJ soft marker band capstone (25891–95600): host soft probes
 * (cgj_soft_milestone_95600.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 95600. */
const char __libcgj_batch95600_marker[] = "libcgj-batch95600";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b95600_id(void)
{
	return 95600u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_95600 - report this TU's graph batch number.
 *
 * Always returns 95600u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 95600.
 */
uint32_t
gj_batch_id_95600(void)
{
	(void)NULL;
	return b95600_id();
}

/*
 * gj_graph_milestone_95600 - report this TU's graph milestone revision.
 *
 * Always returns 95600u (MILESTONE 95600). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 95600.
 */
uint32_t
gj_graph_milestone_95600(void)
{
	return b95600_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_95600(void)
    __attribute__((alias("gj_batch_id_95600")));

uint32_t __gj_graph_milestone_95600(void)
    __attribute__((alias("gj_graph_milestone_95600")));
