/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch29400: MILESTONE 29400 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_29400(void);
 *     - Returns the compile-time graph batch number for this TU (29400).
 *   uint32_t gj_graph_milestone_29400(void);
 *     - Returns the current graph milestone revision (29400).
 *   uint32_t __gj_batch_id_29400  (alias)
 *   uint32_t __gj_graph_milestone_29400  (alias)
 *   __libcgj_batch29400_marker = "libcgj-batch29400"
 *
 * MILESTONE 29400 for the exclusive continuum CREATE-ONLY wave
 * (batches 29391-29399: shell_green_29400, libcgj_green_29400,
 * bar3_ready_29400, product_score_29400, continuum_ready_29400,
 * smoke_soft_29400, dyn_soft_29400, milestone_tag_29400,
 * continuum_wave_29400). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=29400. bar3 remains open (gj_bar3_ready_29400 == 0);
 * product_score remains open (gj_product_score_29400 == 0).
 *
 * CGJ soft marker band capstone (29391–29400): host soft probes
 * (cgj_soft_milestone_29400.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 29400. */
const char __libcgj_batch29400_marker[] = "libcgj-batch29400";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b29400_id(void)
{
	return 29400u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_29400 - report this TU's graph batch number.
 *
 * Always returns 29400u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 29400.
 */
uint32_t
gj_batch_id_29400(void)
{
	(void)NULL;
	return b29400_id();
}

/*
 * gj_graph_milestone_29400 - report this TU's graph milestone revision.
 *
 * Always returns 29400u (MILESTONE 29400). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 29400.
 */
uint32_t
gj_graph_milestone_29400(void)
{
	return b29400_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_29400(void)
    __attribute__((alias("gj_batch_id_29400")));

uint32_t __gj_graph_milestone_29400(void)
    __attribute__((alias("gj_graph_milestone_29400")));
