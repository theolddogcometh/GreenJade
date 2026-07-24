/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch37600: MILESTONE 37600 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_37600(void);
 *     - Returns the compile-time graph batch number for this TU (37600).
 *   uint32_t gj_graph_milestone_37600(void);
 *     - Returns the current graph milestone revision (37600).
 *   uint32_t __gj_batch_id_37600  (alias)
 *   uint32_t __gj_graph_milestone_37600  (alias)
 *   __libcgj_batch37600_marker = "libcgj-batch37600"
 *
 * MILESTONE 37600 for the exclusive continuum CREATE-ONLY wave
 * (batches 37591-37599: shell_green_37600, libcgj_green_37600,
 * bar3_ready_37600, product_score_37600, continuum_ready_37600,
 * smoke_soft_37600, dyn_soft_37600, milestone_tag_37600,
 * continuum_wave_37600). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=37600. bar3 remains open (gj_bar3_ready_37600 == 0);
 * product_score remains open (gj_product_score_37600 == 0).
 *
 * CGJ soft marker band capstone (37591–37600): host soft probes
 * (cgj_soft_milestone_37600.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 37600. */
const char __libcgj_batch37600_marker[] = "libcgj-batch37600";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b37600_id(void)
{
	return 37600u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_37600 - report this TU's graph batch number.
 *
 * Always returns 37600u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 37600.
 */
uint32_t
gj_batch_id_37600(void)
{
	(void)NULL;
	return b37600_id();
}

/*
 * gj_graph_milestone_37600 - report this TU's graph milestone revision.
 *
 * Always returns 37600u (MILESTONE 37600). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 37600.
 */
uint32_t
gj_graph_milestone_37600(void)
{
	return b37600_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_37600(void)
    __attribute__((alias("gj_batch_id_37600")));

uint32_t __gj_graph_milestone_37600(void)
    __attribute__((alias("gj_graph_milestone_37600")));
