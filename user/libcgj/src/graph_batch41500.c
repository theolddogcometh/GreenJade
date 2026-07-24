/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch41500: MILESTONE 41500 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_41500(void);
 *     - Returns the compile-time graph batch number for this TU (41500).
 *   uint32_t gj_graph_milestone_41500(void);
 *     - Returns the current graph milestone revision (41500).
 *   uint32_t __gj_batch_id_41500  (alias)
 *   uint32_t __gj_graph_milestone_41500  (alias)
 *   __libcgj_batch41500_marker = "libcgj-batch41500"
 *
 * MILESTONE 41500 for the exclusive continuum CREATE-ONLY wave
 * (batches 41491-41499: shell_green_41500, libcgj_green_41500,
 * bar3_ready_41500, product_score_41500, continuum_ready_41500,
 * smoke_soft_41500, dyn_soft_41500, milestone_tag_41500,
 * continuum_wave_41500). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=41500. bar3 remains open (gj_bar3_ready_41500 == 0);
 * product_score remains open (gj_product_score_41500 == 0).
 *
 * CGJ soft marker band capstone (41491–41500): host soft probes
 * (cgj_soft_milestone_41500.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 41500. */
const char __libcgj_batch41500_marker[] = "libcgj-batch41500";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b41500_id(void)
{
	return 41500u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_41500 - report this TU's graph batch number.
 *
 * Always returns 41500u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 41500.
 */
uint32_t
gj_batch_id_41500(void)
{
	(void)NULL;
	return b41500_id();
}

/*
 * gj_graph_milestone_41500 - report this TU's graph milestone revision.
 *
 * Always returns 41500u (MILESTONE 41500). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 41500.
 */
uint32_t
gj_graph_milestone_41500(void)
{
	return b41500_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_41500(void)
    __attribute__((alias("gj_batch_id_41500")));

uint32_t __gj_graph_milestone_41500(void)
    __attribute__((alias("gj_graph_milestone_41500")));
