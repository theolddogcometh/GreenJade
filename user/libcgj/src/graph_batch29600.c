/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch29600: MILESTONE 29600 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_29600(void);
 *     - Returns the compile-time graph batch number for this TU (29600).
 *   uint32_t gj_graph_milestone_29600(void);
 *     - Returns the current graph milestone revision (29600).
 *   uint32_t __gj_batch_id_29600  (alias)
 *   uint32_t __gj_graph_milestone_29600  (alias)
 *   __libcgj_batch29600_marker = "libcgj-batch29600"
 *
 * MILESTONE 29600 for the exclusive continuum CREATE-ONLY wave
 * (batches 29591-29599: shell_green_29600, libcgj_green_29600,
 * bar3_ready_29600, product_score_29600, continuum_ready_29600,
 * smoke_soft_29600, dyn_soft_29600, milestone_tag_29600,
 * continuum_wave_29600). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=29600. bar3 remains open (gj_bar3_ready_29600 == 0);
 * product_score remains open (gj_product_score_29600 == 0).
 *
 * CGJ soft marker band capstone (29591–29600): host soft probes
 * (cgj_soft_milestone_29600.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 29600. */
const char __libcgj_batch29600_marker[] = "libcgj-batch29600";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b29600_id(void)
{
	return 29600u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_29600 - report this TU's graph batch number.
 *
 * Always returns 29600u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 29600.
 */
uint32_t
gj_batch_id_29600(void)
{
	(void)NULL;
	return b29600_id();
}

/*
 * gj_graph_milestone_29600 - report this TU's graph milestone revision.
 *
 * Always returns 29600u (MILESTONE 29600). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 29600.
 */
uint32_t
gj_graph_milestone_29600(void)
{
	return b29600_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_29600(void)
    __attribute__((alias("gj_batch_id_29600")));

uint32_t __gj_graph_milestone_29600(void)
    __attribute__((alias("gj_graph_milestone_29600")));
