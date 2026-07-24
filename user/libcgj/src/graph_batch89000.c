/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch89000: MILESTONE 89000 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_89000(void);
 *     - Returns the compile-time graph batch number for this TU (89000).
 *   uint32_t gj_graph_milestone_89000(void);
 *     - Returns the current graph milestone revision (89000).
 *   uint32_t __gj_batch_id_89000  (alias)
 *   uint32_t __gj_graph_milestone_89000  (alias)
 *   __libcgj_batch89000_marker = "libcgj-batch89000"
 *
 * MILESTONE 89000 for the exclusive continuum CREATE-ONLY wave
 * (batches 25891-25899: shell_green_89000, libcgj_green_89000,
 * bar3_ready_89000, product_score_89000, continuum_ready_89000,
 * smoke_soft_89000, dyn_soft_89000, milestone_tag_89000,
 * continuum_wave_89000). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=89000. bar3 remains open (gj_bar3_ready_89000 == 0);
 * product_score remains open (gj_product_score_89000 == 0).
 *
 * CGJ soft marker band capstone (25891–89000): host soft probes
 * (cgj_soft_milestone_89000.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 89000. */
const char __libcgj_batch89000_marker[] = "libcgj-batch89000";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b89000_id(void)
{
	return 89000u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_89000 - report this TU's graph batch number.
 *
 * Always returns 89000u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 89000.
 */
uint32_t
gj_batch_id_89000(void)
{
	(void)NULL;
	return b89000_id();
}

/*
 * gj_graph_milestone_89000 - report this TU's graph milestone revision.
 *
 * Always returns 89000u (MILESTONE 89000). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 89000.
 */
uint32_t
gj_graph_milestone_89000(void)
{
	return b89000_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_89000(void)
    __attribute__((alias("gj_batch_id_89000")));

uint32_t __gj_graph_milestone_89000(void)
    __attribute__((alias("gj_graph_milestone_89000")));
