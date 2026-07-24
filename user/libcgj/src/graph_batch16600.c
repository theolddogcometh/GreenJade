/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch16600: MILESTONE 16600 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_16600(void);
 *     - Returns the compile-time graph batch number for this TU (16600).
 *   uint32_t gj_graph_milestone_16600(void);
 *     - Returns the current graph milestone revision (16600).
 *   uint32_t __gj_batch_id_16600  (alias)
 *   uint32_t __gj_graph_milestone_16600  (alias)
 *   __libcgj_batch16600_marker = "libcgj-batch16600"
 *
 * MILESTONE 16600 for the exclusive continuum CREATE-ONLY wave
 * (batches 16591-16599: shell_green_16600, libcgj_green_16600,
 * bar3_ready_16600, product_score_16600, continuum_ready_16600,
 * smoke_soft_16600, dyn_soft_16600, milestone_tag_16600,
 * continuum_wave_16600). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=16600. bar3 remains open (gj_bar3_ready_16600 == 0);
 * product_score remains open (gj_product_score_16600 == 0).
 *
 * CGJ soft marker band capstone (16591–16600): host soft probes
 * (cgj_soft_milestone_16600.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 16600. */
const char __libcgj_batch16600_marker[] = "libcgj-batch16600";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b16600_id(void)
{
	return 16600u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_16600 - report this TU's graph batch number.
 *
 * Always returns 16600u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 16600.
 */
uint32_t
gj_batch_id_16600(void)
{
	(void)NULL;
	return b16600_id();
}

/*
 * gj_graph_milestone_16600 - report this TU's graph milestone revision.
 *
 * Always returns 16600u (MILESTONE 16600). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 16600.
 */
uint32_t
gj_graph_milestone_16600(void)
{
	return b16600_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_16600(void)
    __attribute__((alias("gj_batch_id_16600")));

uint32_t __gj_graph_milestone_16600(void)
    __attribute__((alias("gj_graph_milestone_16600")));
