/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch46400: MILESTONE 46400 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_46400(void);
 *     - Returns the compile-time graph batch number for this TU (46400).
 *   uint32_t gj_graph_milestone_46400(void);
 *     - Returns the current graph milestone revision (46400).
 *   uint32_t __gj_batch_id_46400  (alias)
 *   uint32_t __gj_graph_milestone_46400  (alias)
 *   __libcgj_batch46400_marker = "libcgj-batch46400"
 *
 * MILESTONE 46400 for the exclusive continuum CREATE-ONLY wave
 * (batches 46391-46399: shell_green_46400, libcgj_green_46400,
 * bar3_ready_46400, product_score_46400, continuum_ready_46400,
 * smoke_soft_46400, dyn_soft_46400, milestone_tag_46400,
 * continuum_wave_46400). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=46400. bar3 remains open (gj_bar3_ready_46400 == 0);
 * product_score remains open (gj_product_score_46400 == 0).
 *
 * CGJ soft marker band capstone (46391–46400): host soft probes
 * (cgj_soft_milestone_46400.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 46400. */
const char __libcgj_batch46400_marker[] = "libcgj-batch46400";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b46400_id(void)
{
	return 46400u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_46400 - report this TU's graph batch number.
 *
 * Always returns 46400u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 46400.
 */
uint32_t
gj_batch_id_46400(void)
{
	(void)NULL;
	return b46400_id();
}

/*
 * gj_graph_milestone_46400 - report this TU's graph milestone revision.
 *
 * Always returns 46400u (MILESTONE 46400). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 46400.
 */
uint32_t
gj_graph_milestone_46400(void)
{
	return b46400_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_46400(void)
    __attribute__((alias("gj_batch_id_46400")));

uint32_t __gj_graph_milestone_46400(void)
    __attribute__((alias("gj_graph_milestone_46400")));
