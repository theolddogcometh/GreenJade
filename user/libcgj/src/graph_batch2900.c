/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2900: milestone 2900 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2900(void);
 *     - Returns the compile-time graph batch number for this TU (2900).
 *   uint32_t gj_graph_milestone_2900(void);
 *     - Returns the current graph milestone revision (2900).
 *   uint32_t __gj_batch_id_2900  (alias)
 *   uint32_t __gj_graph_milestone_2900  (alias)
 *   __libcgj_batch2900_marker = "libcgj-batch2900"
 *
 * Milestone for the exclusive product helpers wave (batches 2891-2899:
 * bar3_install_ok_u, bar3_steam_ok_u, bar3_deck_ok_u, bar3_top50_ok_u,
 * bar3_mask_all_u, product_shell_ok_u, product_libcgj_ok_u,
 * product_smoke_pair_u, product_dyn_pair_u). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2900_marker[] = "libcgj-batch2900";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2900_id(void)
{
	return 2900u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2900 - report this TU's graph batch number.
 *
 * Always returns 2900.
 */
uint32_t
gj_batch_id_2900(void)
{
	(void)NULL;
	return b2900_id();
}

/*
 * gj_graph_milestone_2900 - report this TU's graph milestone revision.
 *
 * Always returns 2900 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2900(void)
{
	return b2900_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2900(void)
    __attribute__((alias("gj_batch_id_2900")));

uint32_t __gj_graph_milestone_2900(void)
    __attribute__((alias("gj_graph_milestone_2900")));
