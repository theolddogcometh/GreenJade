/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch34000: MILESTONE 34000 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_34000(void);
 *     - Returns the compile-time graph batch number for this TU (34000).
 *   uint32_t gj_graph_milestone_34000(void);
 *     - Returns the current graph milestone revision (34000).
 *   uint32_t __gj_batch_id_34000  (alias)
 *   uint32_t __gj_graph_milestone_34000  (alias)
 *   __libcgj_batch34000_marker = "libcgj-batch34000"
 *
 * MILESTONE 34000 for the exclusive continuum CREATE-ONLY wave
 * (batches 33991-33999: shell_green_34000, libcgj_green_34000,
 * bar3_ready_34000, product_score_34000, continuum_ready_34000,
 * smoke_soft_34000, dyn_soft_34000, milestone_tag_34000,
 * continuum_wave_34000). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols - avoid
 * multi-def. No parent wires.
 *
 * Soft continuum capstone only. Honest parent high-water when wired:
 * makefile_max=34000. bar3 remains open (gj_bar3_ready_34000 == 0);
 * product_score remains open (gj_product_score_34000 == 0).
 *
 * CGJ soft marker band capstone (33991–34000): host soft probes
 * (cgj_soft_milestone_34000.*) may dlsym / direct-link these lamps.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 34000. */
const char __libcgj_batch34000_marker[] = "libcgj-batch34000";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b34000_id(void)
{
	return 34000u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_34000 - report this TU's graph batch number.
 *
 * Always returns 34000u. Soft pure-data batch identity lamp for the
 * milestone capstone TU. Does not call libc. No parent wires.
 * CGJ soft KAT expectation: 34000.
 */
uint32_t
gj_batch_id_34000(void)
{
	(void)NULL;
	return b34000_id();
}

/*
 * gj_graph_milestone_34000 - report this TU's graph milestone revision.
 *
 * Always returns 34000u (MILESTONE 34000). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. Soft pure-data only. No parent wires.
 * CGJ soft KAT expectation: 34000.
 */
uint32_t
gj_graph_milestone_34000(void)
{
	return b34000_id();
}

/* ---- underscored aliases ----------------------------------------------- */

/* CGJ aliases: underscored forms for map / weak-link compatibility. */
uint32_t __gj_batch_id_34000(void)
    __attribute__((alias("gj_batch_id_34000")));

uint32_t __gj_graph_milestone_34000(void)
    __attribute__((alias("gj_graph_milestone_34000")));
