/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2370: milestone 2370 batch identity (closes
 * post-2360 product path exclusive wave 2361-2370).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2370(void);
 *     - Returns the compile-time graph batch number for this TU (2370).
 *   uint32_t gj_graph_milestone_2370(void);
 *     - Returns the current graph milestone revision (2370).
 *   uint32_t __gj_batch_id_2370  (alias)
 *   uint32_t __gj_graph_milestone_2370  (alias)
 *   __libcgj_batch2370_marker = "libcgj-batch2370"
 *
 * Milestone for the post-2360 product path exclusive wave (batches
 * 2361-2369: install_stage_advance, install_stage_is_done,
 * rootfs_path_ok, ld_so_name_ok, steam_root_ok, proton_ge_ver_ok,
 * deck_model_ok, game_title_len_ok, launch_args_ok). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2370_marker[] = "libcgj-batch2370";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2370_id(void)
{
	return 2370u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2370 - report this TU's graph batch number.
 *
 * Always returns 2370.
 */
uint32_t
gj_batch_id_2370(void)
{
	(void)NULL;
	return b2370_id();
}

/*
 * gj_graph_milestone_2370 - report this TU's graph milestone revision.
 *
 * Always returns 2370 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2370(void)
{
	return b2370_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2370(void)
    __attribute__((alias("gj_batch_id_2370")));

uint32_t __gj_graph_milestone_2370(void)
    __attribute__((alias("gj_graph_milestone_2370")));
