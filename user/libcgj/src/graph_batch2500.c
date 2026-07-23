/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2500: milestone 2500 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2500(void);
 *     - Returns the compile-time graph batch number for this TU (2500).
 *   uint32_t gj_graph_milestone_2500(void);
 *     - Returns the current graph milestone revision (2500).
 *   uint32_t __gj_batch_id_2500  (alias)
 *   uint32_t __gj_graph_milestone_2500  (alias)
 *   __libcgj_batch2500_marker = "libcgj-batch2500"
 *
 * Milestone for the exclusive product helpers wave (batches 2491-2499:
 * wave_id_2500, export_target_2500, batch_id_ok_range, smoke_pass_p,
 * smoke_fail_p, dyn_pass_p, shell_product_ok, bar3_steam_bit,
 * bar3_deck_bit). Does NOT redefine gj_batch_id / gj_graph_milestone
 * / prior batch_id_* or milestone_* symbols - avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2500_marker[] = "libcgj-batch2500";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2500_id(void)
{
	return 2500u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2500 - report this TU's graph batch number.
 *
 * Always returns 2500.
 */
uint32_t
gj_batch_id_2500(void)
{
	(void)NULL;
	return b2500_id();
}

/*
 * gj_graph_milestone_2500 - report this TU's graph milestone revision.
 *
 * Always returns 2500 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2500(void)
{
	return b2500_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2500(void)
    __attribute__((alias("gj_batch_id_2500")));

uint32_t __gj_graph_milestone_2500(void)
    __attribute__((alias("gj_graph_milestone_2500")));
