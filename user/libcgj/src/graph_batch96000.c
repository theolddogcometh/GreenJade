/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch96000: MILESTONE 96000 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_96000(void);
 *     - Returns the compile-time graph batch number for this TU (96000).
 *   uint32_t gj_graph_milestone_96000(void);
 *     - Returns the current graph milestone revision (96000).
 *   uint32_t __gj_batch_id_96000  (alias)
 *   uint32_t __gj_graph_milestone_96000  (alias)
 *   __libcgj_batch96000_marker = "libcgj-batch96000"
 *
 * MILESTONE 96000 for the exclusive continuum CREATE-ONLY wave
 * (batches 25891-25899: shell_green_96000, libcgj_green_96000,
 * bar3_ready_96000, product_score_96000, continuum_ready_96000,
 * smoke_soft_96000, dyn_soft_96000, milestone_tag_96000,
 * continuum_wave_96000). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=96000. bar3 remains open (gj_bar3_ready_96000 == 0);
 * product_score remains open (gj_product_score_96000 == 0).
 *
 * CGJ soft marker band capstone (25891–96000): host soft probes
 * (cgj_soft_milestone_96000.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 96000. */
const char __libcgj_batch96000_marker[] = "libcgj-batch96000";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b96000_id(void)
{
	return 96000u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_96000 - report this TU's graph batch number.
 *
 * Always returns 96000u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 96000.
 */
uint32_t
gj_batch_id_96000(void)
{
	(void)NULL;
	return b96000_id();
}

/*
 * gj_graph_milestone_96000 - report this TU's graph milestone revision.
 *
 * Always returns 96000u (MILESTONE 96000). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 96000.
 */
uint32_t
gj_graph_milestone_96000(void)
{
	return b96000_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_96000(void)
    __attribute__((alias("gj_batch_id_96000")));

uint32_t __gj_graph_milestone_96000(void)
    __attribute__((alias("gj_graph_milestone_96000")));
