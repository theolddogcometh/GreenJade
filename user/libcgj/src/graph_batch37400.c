/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch37400: MILESTONE 37400 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_37400(void);
 *     - Returns the compile-time graph batch number for this TU (37400).
 *   uint32_t gj_graph_milestone_37400(void);
 *     - Returns the current graph milestone revision (37400).
 *   uint32_t __gj_batch_id_37400  (alias)
 *   uint32_t __gj_graph_milestone_37400  (alias)
 *   __libcgj_batch37400_marker = "libcgj-batch37400"
 *
 * MILESTONE 37400 for the exclusive continuum CREATE-ONLY wave
 * (batches 37391-37399: shell_green_37400, libcgj_green_37400,
 * bar3_ready_37400, product_score_37400, continuum_ready_37400,
 * smoke_soft_37400, dyn_soft_37400, milestone_tag_37400,
 * continuum_wave_37400). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=37400. bar3 remains open (gj_bar3_ready_37400 == 0);
 * product_score remains open (gj_product_score_37400 == 0).
 *
 * CGJ soft marker band capstone (37391–37400): host soft probes
 * (cgj_soft_milestone_37400.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 37400. */
const char __libcgj_batch37400_marker[] = "libcgj-batch37400";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b37400_id(void)
{
	return 37400u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_37400 - report this TU's graph batch number.
 *
 * Always returns 37400u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 37400.
 */
uint32_t
gj_batch_id_37400(void)
{
	(void)NULL;
	return b37400_id();
}

/*
 * gj_graph_milestone_37400 - report this TU's graph milestone revision.
 *
 * Always returns 37400u (MILESTONE 37400). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 37400.
 */
uint32_t
gj_graph_milestone_37400(void)
{
	return b37400_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_37400(void)
    __attribute__((alias("gj_batch_id_37400")));

uint32_t __gj_graph_milestone_37400(void)
    __attribute__((alias("gj_graph_milestone_37400")));
