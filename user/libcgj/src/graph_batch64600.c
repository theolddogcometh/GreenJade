/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch64600: MILESTONE 64600 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_64600(void);
 *     - Returns the compile-time graph batch number for this TU (64600).
 *   uint32_t gj_graph_milestone_64600(void);
 *     - Returns the current graph milestone revision (64600).
 *   uint32_t __gj_batch_id_64600  (alias)
 *   uint32_t __gj_graph_milestone_64600  (alias)
 *   __libcgj_batch64600_marker = "libcgj-batch64600"
 *
 * MILESTONE 64600 for the exclusive continuum CREATE-ONLY wave
 * (batches 25891-25899: shell_green_64600, libcgj_green_64600,
 * bar3_ready_64600, product_score_64600, continuum_ready_64600,
 * smoke_soft_64600, dyn_soft_64600, milestone_tag_64600,
 * continuum_wave_64600). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=64600. bar3 remains open (gj_bar3_ready_64600 == 0);
 * product_score remains open (gj_product_score_64600 == 0).
 *
 * CGJ soft marker band capstone (25891–64600): host soft probes
 * (cgj_soft_milestone_64600.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 64600. */
const char __libcgj_batch64600_marker[] = "libcgj-batch64600";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b64600_id(void)
{
	return 64600u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_64600 - report this TU's graph batch number.
 *
 * Always returns 64600u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 64600.
 */
uint32_t
gj_batch_id_64600(void)
{
	(void)NULL;
	return b64600_id();
}

/*
 * gj_graph_milestone_64600 - report this TU's graph milestone revision.
 *
 * Always returns 64600u (MILESTONE 64600). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 64600.
 */
uint32_t
gj_graph_milestone_64600(void)
{
	return b64600_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_64600(void)
    __attribute__((alias("gj_batch_id_64600")));

uint32_t __gj_graph_milestone_64600(void)
    __attribute__((alias("gj_graph_milestone_64600")));
