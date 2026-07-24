/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch26100: MILESTONE 26100 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_26100(void);
 *     - Returns the compile-time graph batch number for this TU (26100).
 *   uint32_t gj_graph_milestone_26100(void);
 *     - Returns the current graph milestone revision (26100).
 *   uint32_t __gj_batch_id_26100  (alias)
 *   uint32_t __gj_graph_milestone_26100  (alias)
 *   __libcgj_batch26100_marker = "libcgj-batch26100"
 *
 * MILESTONE 26100 for the exclusive continuum CREATE-ONLY wave
 * (batches 26091-26099: shell_green_26100, libcgj_green_26100,
 * bar3_ready_26100, product_score_26100, continuum_ready_26100,
 * smoke_soft_26100, dyn_soft_26100, milestone_tag_26100,
 * continuum_wave_26100). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=26100. bar3 remains open (gj_bar3_ready_26100 == 0);
 * product_score remains open (gj_product_score_26100 == 0).
 *
 * CGJ soft marker band capstone (26091–26100): host soft probes
 * (cgj_soft_milestone_26100.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 26100. */
const char __libcgj_batch26100_marker[] = "libcgj-batch26100";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b26100_id(void)
{
	return 26100u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_26100 - report this TU's graph batch number.
 *
 * Always returns 26100u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 26100.
 */
uint32_t
gj_batch_id_26100(void)
{
	(void)NULL;
	return b26100_id();
}

/*
 * gj_graph_milestone_26100 - report this TU's graph milestone revision.
 *
 * Always returns 26100u (MILESTONE 26100). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 26100.
 */
uint32_t
gj_graph_milestone_26100(void)
{
	return b26100_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_26100(void)
    __attribute__((alias("gj_batch_id_26100")));

uint32_t __gj_graph_milestone_26100(void)
    __attribute__((alias("gj_graph_milestone_26100")));
