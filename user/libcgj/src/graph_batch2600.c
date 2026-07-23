/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2600: milestone 2600 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2600(void);
 *     - Returns the compile-time graph batch number for this TU (2600).
 *   uint32_t gj_graph_milestone_2600(void);
 *     - Returns the current graph milestone revision (2600).
 *   uint32_t __gj_batch_id_2600  (alias)
 *   uint32_t __gj_graph_milestone_2600  (alias)
 *   __libcgj_batch2600_marker = "libcgj-batch2600"
 *
 * Milestone for the exclusive product helpers wave (batches 2591-2599:
 * wave_id_2600, export_hint_2600, batch_chain_ok, smoke_both_pass_p,
 * libcgj_ready_p, shell_ready_p, product_bar3_mask, install_ready_bits,
 * continuum_wave_ok). Does NOT redefine gj_batch_id / gj_graph_milestone
 * / prior batch_id_* or milestone_* symbols - avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2600_marker[] = "libcgj-batch2600";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2600_id(void)
{
	return 2600u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2600 - report this TU's graph batch number.
 *
 * Always returns 2600.
 */
uint32_t
gj_batch_id_2600(void)
{
	(void)NULL;
	return b2600_id();
}

/*
 * gj_graph_milestone_2600 - report this TU's graph milestone revision.
 *
 * Always returns 2600 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2600(void)
{
	return b2600_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2600(void)
    __attribute__((alias("gj_batch_id_2600")));

uint32_t __gj_graph_milestone_2600(void)
    __attribute__((alias("gj_graph_milestone_2600")));
