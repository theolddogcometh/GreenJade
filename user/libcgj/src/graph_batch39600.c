/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch39600: MILESTONE 39600 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_39600(void);
 *     - Returns the compile-time graph batch number for this TU (39600).
 *   uint32_t gj_graph_milestone_39600(void);
 *     - Returns the current graph milestone revision (39600).
 *   uint32_t __gj_batch_id_39600  (alias)
 *   uint32_t __gj_graph_milestone_39600  (alias)
 *   __libcgj_batch39600_marker = "libcgj-batch39600"
 *
 * MILESTONE 39600 for the exclusive continuum CREATE-ONLY wave
 * (batches 39591-39599: shell_green_39600, libcgj_green_39600,
 * bar3_ready_39600, product_score_39600, continuum_ready_39600,
 * smoke_soft_39600, dyn_soft_39600, milestone_tag_39600,
 * continuum_wave_39600). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=39600. bar3 remains open (gj_bar3_ready_39600 == 0);
 * product_score remains open (gj_product_score_39600 == 0).
 *
 * CGJ soft marker band capstone (39591–39600): host soft probes
 * (cgj_soft_milestone_39600.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 39600. */
const char __libcgj_batch39600_marker[] = "libcgj-batch39600";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b39600_id(void)
{
	return 39600u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_39600 - report this TU's graph batch number.
 *
 * Always returns 39600u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 39600.
 */
uint32_t
gj_batch_id_39600(void)
{
	(void)NULL;
	return b39600_id();
}

/*
 * gj_graph_milestone_39600 - report this TU's graph milestone revision.
 *
 * Always returns 39600u (MILESTONE 39600). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 39600.
 */
uint32_t
gj_graph_milestone_39600(void)
{
	return b39600_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_39600(void)
    __attribute__((alias("gj_batch_id_39600")));

uint32_t __gj_graph_milestone_39600(void)
    __attribute__((alias("gj_graph_milestone_39600")));
