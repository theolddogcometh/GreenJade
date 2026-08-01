/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch94400: MILESTONE 94400 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_94400(void);
 *     - Returns the compile-time graph batch number for this TU (94400).
 *   uint32_t gj_graph_milestone_94400(void);
 *     - Returns the current graph milestone revision (94400).
 *   uint32_t __gj_batch_id_94400  (alias)
 *   uint32_t __gj_graph_milestone_94400  (alias)
 *   __libcgj_batch94400_marker = "libcgj-batch94400"
 *
 * MILESTONE 94400 for the exclusive continuum CREATE-ONLY wave
 * (batches 25891-25899: shell_green_94400, libcgj_green_94400,
 * bar3_ready_94400, product_score_94400, continuum_ready_94400,
 * smoke_soft_94400, dyn_soft_94400, milestone_tag_94400,
 * continuum_wave_94400). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=94400. bar3 remains open (gj_bar3_ready_94400 == 0);
 * product_score remains open (gj_product_score_94400 == 0).
 *
 * CGJ soft marker band capstone (25891–94400): host soft probes
 * (cgj_soft_milestone_94400.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 94400. */
const char __libcgj_batch94400_marker[] = "libcgj-batch94400";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b94400_id(void)
{
	return 94400u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_94400 - report this TU's graph batch number.
 *
 * Always returns 94400u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 94400.
 */
uint32_t
gj_batch_id_94400(void)
{
	(void)NULL;
	return b94400_id();
}

/*
 * gj_graph_milestone_94400 - report this TU's graph milestone revision.
 *
 * Always returns 94400u (MILESTONE 94400). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 94400.
 */
uint32_t
gj_graph_milestone_94400(void)
{
	return b94400_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_94400(void)
    __attribute__((alias("gj_batch_id_94400")));

uint32_t __gj_graph_milestone_94400(void)
    __attribute__((alias("gj_graph_milestone_94400")));
