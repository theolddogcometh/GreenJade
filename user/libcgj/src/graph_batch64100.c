/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch64100: MILESTONE 64100 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_64100(void);
 *     - Returns the compile-time graph batch number for this TU (64100).
 *   uint32_t gj_graph_milestone_64100(void);
 *     - Returns the current graph milestone revision (64100).
 *   uint32_t __gj_batch_id_64100  (alias)
 *   uint32_t __gj_graph_milestone_64100  (alias)
 *   __libcgj_batch64100_marker = "libcgj-batch64100"
 *
 * MILESTONE 64100 for the exclusive continuum CREATE-ONLY wave
 * (batches 25891-25899: shell_green_64100, libcgj_green_64100,
 * bar3_ready_64100, product_score_64100, continuum_ready_64100,
 * smoke_soft_64100, dyn_soft_64100, milestone_tag_64100,
 * continuum_wave_64100). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=64100. bar3 remains open (gj_bar3_ready_64100 == 0);
 * product_score remains open (gj_product_score_64100 == 0).
 *
 * CGJ soft marker band capstone (25891–64100): host soft probes
 * (cgj_soft_milestone_64100.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 64100. */
const char __libcgj_batch64100_marker[] = "libcgj-batch64100";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b64100_id(void)
{
	return 64100u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_64100 - report this TU's graph batch number.
 *
 * Always returns 64100u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 64100.
 */
uint32_t
gj_batch_id_64100(void)
{
	(void)NULL;
	return b64100_id();
}

/*
 * gj_graph_milestone_64100 - report this TU's graph milestone revision.
 *
 * Always returns 64100u (MILESTONE 64100). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 64100.
 */
uint32_t
gj_graph_milestone_64100(void)
{
	return b64100_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_64100(void)
    __attribute__((alias("gj_batch_id_64100")));

uint32_t __gj_graph_milestone_64100(void)
    __attribute__((alias("gj_graph_milestone_64100")));
