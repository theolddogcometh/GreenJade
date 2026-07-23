/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch15600: MILESTONE 15600 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_15600(void);
 *     - Returns the compile-time graph batch number for this TU (15600).
 *   uint32_t gj_graph_milestone_15600(void);
 *     - Returns the current graph milestone revision (15600).
 *   uint32_t __gj_batch_id_15600  (alias)
 *   uint32_t __gj_graph_milestone_15600  (alias)
 *   __libcgj_batch15600_marker = "libcgj-batch15600"
 *
 * MILESTONE 15600 for the exclusive continuum CREATE-ONLY wave
 * (batches 15591-15599: shell_green_15600, libcgj_green_15600,
 * bar3_ready_15600, product_score_15600, continuum_ready_15600,
 * smoke_soft_15600, dyn_soft_15600, milestone_tag_15600,
 * continuum_wave_15600). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=15600. bar3 remains open (gj_bar3_ready_15600 == 0);
 * product_score remains open (gj_product_score_15600 == 0).
 *
 * CGJ soft marker band capstone (15591–15600): host soft probes
 * (cgj_soft_milestone_15600.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 15600. */
const char __libcgj_batch15600_marker[] = "libcgj-batch15600";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b15600_id(void)
{
	return 15600u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_15600 - report this TU's graph batch number.
 *
 * Always returns 15600u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 15600.
 */
uint32_t
gj_batch_id_15600(void)
{
	(void)NULL;
	return b15600_id();
}

/*
 * gj_graph_milestone_15600 - report this TU's graph milestone revision.
 *
 * Always returns 15600u (MILESTONE 15600). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 15600.
 */
uint32_t
gj_graph_milestone_15600(void)
{
	return b15600_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_15600(void)
    __attribute__((alias("gj_batch_id_15600")));

uint32_t __gj_graph_milestone_15600(void)
    __attribute__((alias("gj_graph_milestone_15600")));
