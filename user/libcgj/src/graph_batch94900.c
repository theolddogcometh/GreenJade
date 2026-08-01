/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch94900: MILESTONE 94900 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_94900(void);
 *     - Returns the compile-time graph batch number for this TU (94900).
 *   uint32_t gj_graph_milestone_94900(void);
 *     - Returns the current graph milestone revision (94900).
 *   uint32_t __gj_batch_id_94900  (alias)
 *   uint32_t __gj_graph_milestone_94900  (alias)
 *   __libcgj_batch94900_marker = "libcgj-batch94900"
 *
 * MILESTONE 94900 for the exclusive continuum CREATE-ONLY wave
 * (batches 25891-25899: shell_green_94900, libcgj_green_94900,
 * bar3_ready_94900, product_score_94900, continuum_ready_94900,
 * smoke_soft_94900, dyn_soft_94900, milestone_tag_94900,
 * continuum_wave_94900). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=94900. bar3 remains open (gj_bar3_ready_94900 == 0);
 * product_score remains open (gj_product_score_94900 == 0).
 *
 * CGJ soft marker band capstone (25891–94900): host soft probes
 * (cgj_soft_milestone_94900.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 94900. */
const char __libcgj_batch94900_marker[] = "libcgj-batch94900";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b94900_id(void)
{
	return 94900u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_94900 - report this TU's graph batch number.
 *
 * Always returns 94900u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 94900.
 */
uint32_t
gj_batch_id_94900(void)
{
	(void)NULL;
	return b94900_id();
}

/*
 * gj_graph_milestone_94900 - report this TU's graph milestone revision.
 *
 * Always returns 94900u (MILESTONE 94900). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 94900.
 */
uint32_t
gj_graph_milestone_94900(void)
{
	return b94900_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_94900(void)
    __attribute__((alias("gj_batch_id_94900")));

uint32_t __gj_graph_milestone_94900(void)
    __attribute__((alias("gj_graph_milestone_94900")));
