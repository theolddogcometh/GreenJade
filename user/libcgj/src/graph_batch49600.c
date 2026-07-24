/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch49600: MILESTONE 49600 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_49600(void);
 *     - Returns the compile-time graph batch number for this TU (49600).
 *   uint32_t gj_graph_milestone_49600(void);
 *     - Returns the current graph milestone revision (49600).
 *   uint32_t __gj_batch_id_49600  (alias)
 *   uint32_t __gj_graph_milestone_49600  (alias)
 *   __libcgj_batch49600_marker = "libcgj-batch49600"
 *
 * MILESTONE 49600 for the exclusive continuum CREATE-ONLY wave
 * (batches 49591-49599: shell_green_49600, libcgj_green_49600,
 * bar3_ready_49600, product_score_49600, continuum_ready_49600,
 * smoke_soft_49600, dyn_soft_49600, milestone_tag_49600,
 * continuum_wave_49600). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=49600. bar3 remains open (gj_bar3_ready_49600 == 0);
 * product_score remains open (gj_product_score_49600 == 0).
 *
 * CGJ soft marker band capstone (49591–49600): host soft probes
 * (cgj_soft_milestone_49600.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 49600. */
const char __libcgj_batch49600_marker[] = "libcgj-batch49600";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b49600_id(void)
{
	return 49600u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_49600 - report this TU's graph batch number.
 *
 * Always returns 49600u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 49600.
 */
uint32_t
gj_batch_id_49600(void)
{
	(void)NULL;
	return b49600_id();
}

/*
 * gj_graph_milestone_49600 - report this TU's graph milestone revision.
 *
 * Always returns 49600u (MILESTONE 49600). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 49600.
 */
uint32_t
gj_graph_milestone_49600(void)
{
	return b49600_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_49600(void)
    __attribute__((alias("gj_batch_id_49600")));

uint32_t __gj_graph_milestone_49600(void)
    __attribute__((alias("gj_graph_milestone_49600")));
