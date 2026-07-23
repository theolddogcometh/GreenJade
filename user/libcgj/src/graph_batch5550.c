/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5550: MILESTONE 5550 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5550(void);
 *     - Returns the compile-time graph batch number for this TU (5550).
 *   uint32_t gj_graph_milestone_5550(void);
 *     - Returns the current graph milestone revision (5550).
 *   uint32_t __gj_batch_id_5550  (alias)
 *   uint32_t __gj_graph_milestone_5550  (alias)
 *   __libcgj_batch5550_marker = "libcgj-batch5550"
 *
 * MILESTONE 5550 for the exclusive continuum CREATE-ONLY wave
 * (batches 5541-5549: cef_stub_ready_5541, steamwebhelper_stub_5542,
 * steamwebhelper_ready_5543, browser_overlay_ready_5544,
 * browser_overlay_layer_5545, store_page_ready_5546,
 * store_page_dom_ready_5547, store_page_score_5548,
 * cef_overlay_continuum_ready_5549). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5550_marker[] = "libcgj-batch5550";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5550_id(void)
{
	return 5550u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_5550 - report this TU's graph batch number.
 *
 * Always returns 5550.
 */
uint32_t
gj_batch_id_5550(void)
{
	(void)NULL;
	return b5550_id();
}

/*
 * gj_graph_milestone_5550 - report this TU's graph milestone revision.
 *
 * Always returns 5550 (MILESTONE 5550). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_5550(void)
{
	return b5550_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_5550(void)
    __attribute__((alias("gj_batch_id_5550")));

uint32_t __gj_graph_milestone_5550(void)
    __attribute__((alias("gj_graph_milestone_5550")));
