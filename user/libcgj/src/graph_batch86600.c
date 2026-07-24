/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch86600: MILESTONE 86600 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_86600(void);
 *     - Returns the compile-time graph batch number for this TU (86600).
 *   uint32_t gj_graph_milestone_86600(void);
 *     - Returns the current graph milestone revision (86600).
 *   uint32_t __gj_batch_id_86600  (alias)
 *   uint32_t __gj_graph_milestone_86600  (alias)
 *   __libcgj_batch86600_marker = "libcgj-batch86600"
 *
 * MILESTONE 86600 for the exclusive continuum CREATE-ONLY wave
 * (batches 25891-25899: shell_green_86600, libcgj_green_86600,
 * bar3_ready_86600, product_score_86600, continuum_ready_86600,
 * smoke_soft_86600, dyn_soft_86600, milestone_tag_86600,
 * continuum_wave_86600). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=86600. bar3 remains open (gj_bar3_ready_86600 == 0);
 * product_score remains open (gj_product_score_86600 == 0).
 *
 * CGJ soft marker band capstone (25891–86600): host soft probes
 * (cgj_soft_milestone_86600.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 86600. */
const char __libcgj_batch86600_marker[] = "libcgj-batch86600";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b86600_id(void)
{
	return 86600u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_86600 - report this TU's graph batch number.
 *
 * Always returns 86600u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 86600.
 */
uint32_t
gj_batch_id_86600(void)
{
	(void)NULL;
	return b86600_id();
}

/*
 * gj_graph_milestone_86600 - report this TU's graph milestone revision.
 *
 * Always returns 86600u (MILESTONE 86600). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 86600.
 */
uint32_t
gj_graph_milestone_86600(void)
{
	return b86600_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_86600(void)
    __attribute__((alias("gj_batch_id_86600")));

uint32_t __gj_graph_milestone_86600(void)
    __attribute__((alias("gj_graph_milestone_86600")));
