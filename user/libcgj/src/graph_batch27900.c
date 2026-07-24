/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch27900: MILESTONE 27900 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_27900(void);
 *     - Returns the compile-time graph batch number for this TU (27900).
 *   uint32_t gj_graph_milestone_27900(void);
 *     - Returns the current graph milestone revision (27900).
 *   uint32_t __gj_batch_id_27900  (alias)
 *   uint32_t __gj_graph_milestone_27900  (alias)
 *   __libcgj_batch27900_marker = "libcgj-batch27900"
 *
 * MILESTONE 27900 for the exclusive continuum CREATE-ONLY wave
 * (batches 27891-27899: shell_green_27900, libcgj_green_27900,
 * bar3_ready_27900, product_score_27900, continuum_ready_27900,
 * smoke_soft_27900, dyn_soft_27900, milestone_tag_27900,
 * continuum_wave_27900). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=27900. bar3 remains open (gj_bar3_ready_27900 == 0);
 * product_score remains open (gj_product_score_27900 == 0).
 *
 * CGJ soft marker band capstone (27891–27900): host soft probes
 * (cgj_soft_milestone_27900.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 27900. */
const char __libcgj_batch27900_marker[] = "libcgj-batch27900";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b27900_id(void)
{
	return 27900u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_27900 - report this TU's graph batch number.
 *
 * Always returns 27900u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 27900.
 */
uint32_t
gj_batch_id_27900(void)
{
	(void)NULL;
	return b27900_id();
}

/*
 * gj_graph_milestone_27900 - report this TU's graph milestone revision.
 *
 * Always returns 27900u (MILESTONE 27900). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 27900.
 */
uint32_t
gj_graph_milestone_27900(void)
{
	return b27900_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_27900(void)
    __attribute__((alias("gj_batch_id_27900")));

uint32_t __gj_graph_milestone_27900(void)
    __attribute__((alias("gj_graph_milestone_27900")));
