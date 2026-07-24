/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch63300: MILESTONE 63300 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_63300(void);
 *     - Returns the compile-time graph batch number for this TU (63300).
 *   uint32_t gj_graph_milestone_63300(void);
 *     - Returns the current graph milestone revision (63300).
 *   uint32_t __gj_batch_id_63300  (alias)
 *   uint32_t __gj_graph_milestone_63300  (alias)
 *   __libcgj_batch63300_marker = "libcgj-batch63300"
 *
 * MILESTONE 63300 for the exclusive continuum CREATE-ONLY wave
 * (batches 25891-25899: shell_green_63300, libcgj_green_63300,
 * bar3_ready_63300, product_score_63300, continuum_ready_63300,
 * smoke_soft_63300, dyn_soft_63300, milestone_tag_63300,
 * continuum_wave_63300). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=63300. bar3 remains open (gj_bar3_ready_63300 == 0);
 * product_score remains open (gj_product_score_63300 == 0).
 *
 * CGJ soft marker band capstone (25891–63300): host soft probes
 * (cgj_soft_milestone_63300.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 63300. */
const char __libcgj_batch63300_marker[] = "libcgj-batch63300";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b63300_id(void)
{
	return 63300u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_63300 - report this TU's graph batch number.
 *
 * Always returns 63300u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 63300.
 */
uint32_t
gj_batch_id_63300(void)
{
	(void)NULL;
	return b63300_id();
}

/*
 * gj_graph_milestone_63300 - report this TU's graph milestone revision.
 *
 * Always returns 63300u (MILESTONE 63300). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 63300.
 */
uint32_t
gj_graph_milestone_63300(void)
{
	return b63300_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_63300(void)
    __attribute__((alias("gj_batch_id_63300")));

uint32_t __gj_graph_milestone_63300(void)
    __attribute__((alias("gj_graph_milestone_63300")));
