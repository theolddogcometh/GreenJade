/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch50600: MILESTONE 50600 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_50600(void);
 *     - Returns the compile-time graph batch number for this TU (50600).
 *   uint32_t gj_graph_milestone_50600(void);
 *     - Returns the current graph milestone revision (50600).
 *   uint32_t __gj_batch_id_50600  (alias)
 *   uint32_t __gj_graph_milestone_50600  (alias)
 *   __libcgj_batch50600_marker = "libcgj-batch50600"
 *
 * MILESTONE 50600 for the exclusive continuum CREATE-ONLY wave
 * (batches 50591-50599: shell_green_50600, libcgj_green_50600,
 * bar3_ready_50600, product_score_50600, continuum_ready_50600,
 * smoke_soft_50600, dyn_soft_50600, milestone_tag_50600,
 * continuum_wave_50600). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=50600. bar3 remains open (gj_bar3_ready_50600 == 0);
 * product_score remains open (gj_product_score_50600 == 0).
 *
 * CGJ soft marker band capstone (50591–50600): host soft probes
 * (cgj_soft_milestone_50600.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 50600. */
const char __libcgj_batch50600_marker[] = "libcgj-batch50600";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b50600_id(void)
{
	return 50600u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_50600 - report this TU's graph batch number.
 *
 * Always returns 50600u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 50600.
 */
uint32_t
gj_batch_id_50600(void)
{
	(void)NULL;
	return b50600_id();
}

/*
 * gj_graph_milestone_50600 - report this TU's graph milestone revision.
 *
 * Always returns 50600u (MILESTONE 50600). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 50600.
 */
uint32_t
gj_graph_milestone_50600(void)
{
	return b50600_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_50600(void)
    __attribute__((alias("gj_batch_id_50600")));

uint32_t __gj_graph_milestone_50600(void)
    __attribute__((alias("gj_graph_milestone_50600")));
