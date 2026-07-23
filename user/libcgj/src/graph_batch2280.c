/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2280: milestone 2280 batch identity (closes
 * post-2270 product exclusive wave 2271-2280).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2280(void);
 *     - Returns the compile-time graph batch number for this TU (2280).
 *   uint32_t gj_graph_milestone_2280(void);
 *     - Returns the current graph milestone revision (2280).
 *   uint32_t __gj_batch_id_2280  (alias)
 *   uint32_t __gj_graph_milestone_2280  (alias)
 *   __libcgj_batch2280_marker = "libcgj-batch2280"
 *
 * Milestone for the post-2270 product exclusive wave (batches 2271-2279:
 * steam_appid_ok_u, proton_prefix_ok_u, wine_prefix_ok_u,
 * deck_res_scale_u, fps_cap_ok_u, hdr_mode_ok_u, refresh_prefer_hz,
 * audio_rate_prefer, controller_slot_ok_u). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2280_marker[] = "libcgj-batch2280";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2280_id(void)
{
	return 2280u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2280 - report this TU's graph batch number.
 *
 * Always returns 2280.
 */
uint32_t
gj_batch_id_2280(void)
{
	(void)NULL;
	return b2280_id();
}

/*
 * gj_graph_milestone_2280 - report this TU's graph milestone revision.
 *
 * Always returns 2280 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2280(void)
{
	return b2280_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2280(void)
    __attribute__((alias("gj_batch_id_2280")));

uint32_t __gj_graph_milestone_2280(void)
    __attribute__((alias("gj_graph_milestone_2280")));
