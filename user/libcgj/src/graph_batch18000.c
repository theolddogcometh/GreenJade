/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch18000: MILESTONE 18000 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_18000(void);
 *     - Returns the compile-time graph batch number for this TU (18000).
 *   uint32_t gj_graph_milestone_18000(void);
 *     - Returns the current graph milestone revision (18000).
 *   uint32_t __gj_batch_id_18000  (alias)
 *   uint32_t __gj_graph_milestone_18000  (alias)
 *   __libcgj_batch18000_marker = "libcgj-batch18000"
 *
 * MILESTONE 18000 for the exclusive continuum CREATE-ONLY wave
 * (batches 17991-17999: shell_green_18000, libcgj_green_18000,
 * bar3_ready_18000, product_score_18000, continuum_ready_18000,
 * smoke_soft_18000, dyn_soft_18000, milestone_tag_18000,
 * continuum_wave_18000). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=18000. bar3 remains open (gj_bar3_ready_18000 == 0);
 * product_score remains open (gj_product_score_18000 == 0).
 *
 * CGJ soft marker band capstone (17991–18000): host soft probes
 * (cgj_soft_milestone_18000.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 18000. */
const char __libcgj_batch18000_marker[] = "libcgj-batch18000";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b18000_id(void)
{
	return 18000u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_18000 - report this TU's graph batch number.
 *
 * Always returns 18000u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 18000.
 */
uint32_t
gj_batch_id_18000(void)
{
	(void)NULL;
	return b18000_id();
}

/*
 * gj_graph_milestone_18000 - report this TU's graph milestone revision.
 *
 * Always returns 18000u (MILESTONE 18000). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 18000.
 */
uint32_t
gj_graph_milestone_18000(void)
{
	return b18000_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_18000(void)
    __attribute__((alias("gj_batch_id_18000")));

uint32_t __gj_graph_milestone_18000(void)
    __attribute__((alias("gj_graph_milestone_18000")));
