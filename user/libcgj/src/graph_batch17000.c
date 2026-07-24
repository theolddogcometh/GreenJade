/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch17000: MILESTONE 17000 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_17000(void);
 *     - Returns the compile-time graph batch number for this TU (17000).
 *   uint32_t gj_graph_milestone_17000(void);
 *     - Returns the current graph milestone revision (17000).
 *   uint32_t __gj_batch_id_17000  (alias)
 *   uint32_t __gj_graph_milestone_17000  (alias)
 *   __libcgj_batch17000_marker = "libcgj-batch17000"
 *
 * MILESTONE 17000 for the exclusive continuum CREATE-ONLY wave
 * (batches 16991-16999: shell_green_17000, libcgj_green_17000,
 * bar3_ready_17000, product_score_17000, continuum_ready_17000,
 * smoke_soft_17000, dyn_soft_17000, milestone_tag_17000,
 * continuum_wave_17000). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=17000. bar3 remains open (gj_bar3_ready_17000 == 0);
 * product_score remains open (gj_product_score_17000 == 0).
 *
 * CGJ soft marker band capstone (16991–17000): host soft probes
 * (cgj_soft_milestone_17000.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 17000. */
const char __libcgj_batch17000_marker[] = "libcgj-batch17000";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b17000_id(void)
{
	return 17000u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_17000 - report this TU's graph batch number.
 *
 * Always returns 17000u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 17000.
 */
uint32_t
gj_batch_id_17000(void)
{
	(void)NULL;
	return b17000_id();
}

/*
 * gj_graph_milestone_17000 - report this TU's graph milestone revision.
 *
 * Always returns 17000u (MILESTONE 17000). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 17000.
 */
uint32_t
gj_graph_milestone_17000(void)
{
	return b17000_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_17000(void)
    __attribute__((alias("gj_batch_id_17000")));

uint32_t __gj_graph_milestone_17000(void)
    __attribute__((alias("gj_graph_milestone_17000")));
