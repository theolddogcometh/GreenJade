/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch61400: MILESTONE 61400 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_61400(void);
 *     - Returns the compile-time graph batch number for this TU (61400).
 *   uint32_t gj_graph_milestone_61400(void);
 *     - Returns the current graph milestone revision (61400).
 *   uint32_t __gj_batch_id_61400  (alias)
 *   uint32_t __gj_graph_milestone_61400  (alias)
 *   __libcgj_batch61400_marker = "libcgj-batch61400"
 *
 * MILESTONE 61400 for the exclusive continuum CREATE-ONLY wave
 * (batches 25891-25899: shell_green_61400, libcgj_green_61400,
 * bar3_ready_61400, product_score_61400, continuum_ready_61400,
 * smoke_soft_61400, dyn_soft_61400, milestone_tag_61400,
 * continuum_wave_61400). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=61400. bar3 remains open (gj_bar3_ready_61400 == 0);
 * product_score remains open (gj_product_score_61400 == 0).
 *
 * CGJ soft marker band capstone (25891–61400): host soft probes
 * (cgj_soft_milestone_61400.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 61400. */
const char __libcgj_batch61400_marker[] = "libcgj-batch61400";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b61400_id(void)
{
	return 61400u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_61400 - report this TU's graph batch number.
 *
 * Always returns 61400u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 61400.
 */
uint32_t
gj_batch_id_61400(void)
{
	(void)NULL;
	return b61400_id();
}

/*
 * gj_graph_milestone_61400 - report this TU's graph milestone revision.
 *
 * Always returns 61400u (MILESTONE 61400). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 61400.
 */
uint32_t
gj_graph_milestone_61400(void)
{
	return b61400_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_61400(void)
    __attribute__((alias("gj_batch_id_61400")));

uint32_t __gj_graph_milestone_61400(void)
    __attribute__((alias("gj_graph_milestone_61400")));
