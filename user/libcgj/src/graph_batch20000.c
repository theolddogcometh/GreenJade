/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch20000: MILESTONE 20000 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_20000(void);
 *     - Returns the compile-time graph batch number for this TU (20000).
 *   uint32_t gj_graph_milestone_20000(void);
 *     - Returns the current graph milestone revision (20000).
 *   uint32_t __gj_batch_id_20000  (alias)
 *   uint32_t __gj_graph_milestone_20000  (alias)
 *   __libcgj_batch20000_marker = "libcgj-batch20000"
 *
 * MILESTONE 20000 for the exclusive continuum CREATE-ONLY wave
 * (batches 19991-19999: shell_green_20000, libcgj_green_20000,
 * bar3_ready_20000, product_score_20000, continuum_ready_20000,
 * smoke_soft_20000, dyn_soft_20000, milestone_tag_20000,
 * continuum_wave_20000). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=20000. bar3 remains open (gj_bar3_ready_20000 == 0);
 * product_score remains open (gj_product_score_20000 == 0).
 *
 * CGJ soft marker band capstone (19991–20000): host soft probes
 * (cgj_soft_milestone_20000.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 20000. */
const char __libcgj_batch20000_marker[] = "libcgj-batch20000";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b20000_id(void)
{
	return 20000u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_20000 - report this TU's graph batch number.
 *
 * Always returns 20000u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 20000.
 */
uint32_t
gj_batch_id_20000(void)
{
	(void)NULL;
	return b20000_id();
}

/*
 * gj_graph_milestone_20000 - report this TU's graph milestone revision.
 *
 * Always returns 20000u (MILESTONE 20000). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 20000.
 */
uint32_t
gj_graph_milestone_20000(void)
{
	return b20000_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_20000(void)
    __attribute__((alias("gj_batch_id_20000")));

uint32_t __gj_graph_milestone_20000(void)
    __attribute__((alias("gj_graph_milestone_20000")));
