/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2640: milestone 2640 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2640(void);
 *     - Returns the compile-time graph batch number for this TU (2640).
 *   uint32_t gj_graph_milestone_2640(void);
 *     - Returns the current graph milestone revision (2640).
 *   uint32_t __gj_batch_id_2640  (alias)
 *   uint32_t __gj_graph_milestone_2640  (alias)
 *   __libcgj_batch2640_marker = "libcgj-batch2640"
 *
 * Milestone for the product path exclusive wave (batches 2631-2639:
 * rootfs_marker_ok, ld_gj_name_ok, steam_bin_ok, proton_run_ok,
 * deck_top50_hint_p, game_id_ok_u, launch_env_ok, install_media_ok,
 * product_ready_mask). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols —
 * avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2640_marker[] = "libcgj-batch2640";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2640_id(void)
{
	return 2640u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2640 - report this TU's graph batch number.
 *
 * Always returns 2640.
 */
uint32_t
gj_batch_id_2640(void)
{
	(void)NULL;
	return b2640_id();
}

/*
 * gj_graph_milestone_2640 - report this TU's graph milestone revision.
 *
 * Always returns 2640 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2640(void)
{
	return b2640_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2640(void)
    __attribute__((alias("gj_batch_id_2640")));

uint32_t __gj_graph_milestone_2640(void)
    __attribute__((alias("gj_graph_milestone_2640")));
