/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch27700: MILESTONE 27700 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_27700(void);
 *     - Returns the compile-time graph batch number for this TU (27700).
 *   uint32_t gj_graph_milestone_27700(void);
 *     - Returns the current graph milestone revision (27700).
 *   uint32_t __gj_batch_id_27700  (alias)
 *   uint32_t __gj_graph_milestone_27700  (alias)
 *   __libcgj_batch27700_marker = "libcgj-batch27700"
 *
 * MILESTONE 27700 for the exclusive continuum CREATE-ONLY wave
 * (batches 27691-27699: shell_green_27700, libcgj_green_27700,
 * bar3_ready_27700, product_score_27700, continuum_ready_27700,
 * smoke_soft_27700, dyn_soft_27700, milestone_tag_27700,
 * continuum_wave_27700). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=27700. bar3 remains open (gj_bar3_ready_27700 == 0);
 * product_score remains open (gj_product_score_27700 == 0).
 *
 * CGJ soft marker band capstone (27691–27700): host soft probes
 * (cgj_soft_milestone_27700.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 27700. */
const char __libcgj_batch27700_marker[] = "libcgj-batch27700";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b27700_id(void)
{
	return 27700u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_27700 - report this TU's graph batch number.
 *
 * Always returns 27700u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 27700.
 */
uint32_t
gj_batch_id_27700(void)
{
	(void)NULL;
	return b27700_id();
}

/*
 * gj_graph_milestone_27700 - report this TU's graph milestone revision.
 *
 * Always returns 27700u (MILESTONE 27700). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 27700.
 */
uint32_t
gj_graph_milestone_27700(void)
{
	return b27700_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_27700(void)
    __attribute__((alias("gj_batch_id_27700")));

uint32_t __gj_graph_milestone_27700(void)
    __attribute__((alias("gj_graph_milestone_27700")));
