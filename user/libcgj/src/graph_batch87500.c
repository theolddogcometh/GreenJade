/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch87500: MILESTONE 87500 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_87500(void);
 *     - Returns the compile-time graph batch number for this TU (87500).
 *   uint32_t gj_graph_milestone_87500(void);
 *     - Returns the current graph milestone revision (87500).
 *   uint32_t __gj_batch_id_87500  (alias)
 *   uint32_t __gj_graph_milestone_87500  (alias)
 *   __libcgj_batch87500_marker = "libcgj-batch87500"
 *
 * MILESTONE 87500 for the exclusive continuum CREATE-ONLY wave
 * (batches 25891-25899: shell_green_87500, libcgj_green_87500,
 * bar3_ready_87500, product_score_87500, continuum_ready_87500,
 * smoke_soft_87500, dyn_soft_87500, milestone_tag_87500,
 * continuum_wave_87500). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=87500. bar3 remains open (gj_bar3_ready_87500 == 0);
 * product_score remains open (gj_product_score_87500 == 0).
 *
 * CGJ soft marker band capstone (25891–87500): host soft probes
 * (cgj_soft_milestone_87500.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 87500. */
const char __libcgj_batch87500_marker[] = "libcgj-batch87500";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b87500_id(void)
{
	return 87500u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_87500 - report this TU's graph batch number.
 *
 * Always returns 87500u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 87500.
 */
uint32_t
gj_batch_id_87500(void)
{
	(void)NULL;
	return b87500_id();
}

/*
 * gj_graph_milestone_87500 - report this TU's graph milestone revision.
 *
 * Always returns 87500u (MILESTONE 87500). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 87500.
 */
uint32_t
gj_graph_milestone_87500(void)
{
	return b87500_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_87500(void)
    __attribute__((alias("gj_batch_id_87500")));

uint32_t __gj_graph_milestone_87500(void)
    __attribute__((alias("gj_graph_milestone_87500")));
