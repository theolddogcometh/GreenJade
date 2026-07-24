/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch44300: MILESTONE 44300 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_44300(void);
 *     - Returns the compile-time graph batch number for this TU (44300).
 *   uint32_t gj_graph_milestone_44300(void);
 *     - Returns the current graph milestone revision (44300).
 *   uint32_t __gj_batch_id_44300  (alias)
 *   uint32_t __gj_graph_milestone_44300  (alias)
 *   __libcgj_batch44300_marker = "libcgj-batch44300"
 *
 * MILESTONE 44300 for the exclusive continuum CREATE-ONLY wave
 * (batches 44291-44299: shell_green_44300, libcgj_green_44300,
 * bar3_ready_44300, product_score_44300, continuum_ready_44300,
 * smoke_soft_44300, dyn_soft_44300, milestone_tag_44300,
 * continuum_wave_44300). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=44300. bar3 remains open (gj_bar3_ready_44300 == 0);
 * product_score remains open (gj_product_score_44300 == 0).
 *
 * CGJ soft marker band capstone (44291–44300): host soft probes
 * (cgj_soft_milestone_44300.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 44300. */
const char __libcgj_batch44300_marker[] = "libcgj-batch44300";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b44300_id(void)
{
	return 44300u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_44300 - report this TU's graph batch number.
 *
 * Always returns 44300u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 44300.
 */
uint32_t
gj_batch_id_44300(void)
{
	(void)NULL;
	return b44300_id();
}

/*
 * gj_graph_milestone_44300 - report this TU's graph milestone revision.
 *
 * Always returns 44300u (MILESTONE 44300). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 44300.
 */
uint32_t
gj_graph_milestone_44300(void)
{
	return b44300_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_44300(void)
    __attribute__((alias("gj_batch_id_44300")));

uint32_t __gj_graph_milestone_44300(void)
    __attribute__((alias("gj_graph_milestone_44300")));
