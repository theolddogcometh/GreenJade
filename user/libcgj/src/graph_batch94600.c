/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch94600: MILESTONE 94600 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_94600(void);
 *     - Returns the compile-time graph batch number for this TU (94600).
 *   uint32_t gj_graph_milestone_94600(void);
 *     - Returns the current graph milestone revision (94600).
 *   uint32_t __gj_batch_id_94600  (alias)
 *   uint32_t __gj_graph_milestone_94600  (alias)
 *   __libcgj_batch94600_marker = "libcgj-batch94600"
 *
 * MILESTONE 94600 for the exclusive continuum CREATE-ONLY wave
 * (batches 25891-25899: shell_green_94600, libcgj_green_94600,
 * bar3_ready_94600, product_score_94600, continuum_ready_94600,
 * smoke_soft_94600, dyn_soft_94600, milestone_tag_94600,
 * continuum_wave_94600). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=94600. bar3 remains open (gj_bar3_ready_94600 == 0);
 * product_score remains open (gj_product_score_94600 == 0).
 *
 * CGJ soft marker band capstone (25891–94600): host soft probes
 * (cgj_soft_milestone_94600.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 94600. */
const char __libcgj_batch94600_marker[] = "libcgj-batch94600";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b94600_id(void)
{
	return 94600u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_94600 - report this TU's graph batch number.
 *
 * Always returns 94600u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 94600.
 */
uint32_t
gj_batch_id_94600(void)
{
	(void)NULL;
	return b94600_id();
}

/*
 * gj_graph_milestone_94600 - report this TU's graph milestone revision.
 *
 * Always returns 94600u (MILESTONE 94600). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 94600.
 */
uint32_t
gj_graph_milestone_94600(void)
{
	return b94600_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_94600(void)
    __attribute__((alias("gj_batch_id_94600")));

uint32_t __gj_graph_milestone_94600(void)
    __attribute__((alias("gj_graph_milestone_94600")));
