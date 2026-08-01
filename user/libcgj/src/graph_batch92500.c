/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch92500: MILESTONE 92500 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_92500(void);
 *     - Returns the compile-time graph batch number for this TU (92500).
 *   uint32_t gj_graph_milestone_92500(void);
 *     - Returns the current graph milestone revision (92500).
 *   uint32_t __gj_batch_id_92500  (alias)
 *   uint32_t __gj_graph_milestone_92500  (alias)
 *   __libcgj_batch92500_marker = "libcgj-batch92500"
 *
 * MILESTONE 92500 for the exclusive continuum CREATE-ONLY wave
 * (batches 25891-25899: shell_green_92500, libcgj_green_92500,
 * bar3_ready_92500, product_score_92500, continuum_ready_92500,
 * smoke_soft_92500, dyn_soft_92500, milestone_tag_92500,
 * continuum_wave_92500). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=92500. bar3 remains open (gj_bar3_ready_92500 == 0);
 * product_score remains open (gj_product_score_92500 == 0).
 *
 * CGJ soft marker band capstone (25891–92500): host soft probes
 * (cgj_soft_milestone_92500.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 92500. */
const char __libcgj_batch92500_marker[] = "libcgj-batch92500";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b92500_id(void)
{
	return 92500u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_92500 - report this TU's graph batch number.
 *
 * Always returns 92500u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 92500.
 */
uint32_t
gj_batch_id_92500(void)
{
	(void)NULL;
	return b92500_id();
}

/*
 * gj_graph_milestone_92500 - report this TU's graph milestone revision.
 *
 * Always returns 92500u (MILESTONE 92500). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 92500.
 */
uint32_t
gj_graph_milestone_92500(void)
{
	return b92500_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_92500(void)
    __attribute__((alias("gj_batch_id_92500")));

uint32_t __gj_graph_milestone_92500(void)
    __attribute__((alias("gj_graph_milestone_92500")));
