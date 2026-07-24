/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch84300: MILESTONE 84300 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_84300(void);
 *     - Returns the compile-time graph batch number for this TU (84300).
 *   uint32_t gj_graph_milestone_84300(void);
 *     - Returns the current graph milestone revision (84300).
 *   uint32_t __gj_batch_id_84300  (alias)
 *   uint32_t __gj_graph_milestone_84300  (alias)
 *   __libcgj_batch84300_marker = "libcgj-batch84300"
 *
 * MILESTONE 84300 for the exclusive continuum CREATE-ONLY wave
 * (batches 25891-25899: shell_green_84300, libcgj_green_84300,
 * bar3_ready_84300, product_score_84300, continuum_ready_84300,
 * smoke_soft_84300, dyn_soft_84300, milestone_tag_84300,
 * continuum_wave_84300). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=84300. bar3 remains open (gj_bar3_ready_84300 == 0);
 * product_score remains open (gj_product_score_84300 == 0).
 *
 * CGJ soft marker band capstone (25891–84300): host soft probes
 * (cgj_soft_milestone_84300.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 84300. */
const char __libcgj_batch84300_marker[] = "libcgj-batch84300";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b84300_id(void)
{
	return 84300u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_84300 - report this TU's graph batch number.
 *
 * Always returns 84300u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 84300.
 */
uint32_t
gj_batch_id_84300(void)
{
	(void)NULL;
	return b84300_id();
}

/*
 * gj_graph_milestone_84300 - report this TU's graph milestone revision.
 *
 * Always returns 84300u (MILESTONE 84300). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 84300.
 */
uint32_t
gj_graph_milestone_84300(void)
{
	return b84300_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_84300(void)
    __attribute__((alias("gj_batch_id_84300")));

uint32_t __gj_graph_milestone_84300(void)
    __attribute__((alias("gj_graph_milestone_84300")));
