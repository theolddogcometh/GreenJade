/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2540: milestone 2540 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2540(void);
 *     — Returns the compile-time graph batch number for this TU (2540).
 *   uint32_t gj_graph_milestone_2540(void);
 *     — Returns the current graph milestone revision (2540).
 *   uint32_t __gj_batch_id_2540  (alias)
 *   uint32_t __gj_graph_milestone_2540  (alias)
 *   __libcgj_batch2540_marker = "libcgj-batch2540"
 *
 * Milestone for the time exclusive pure-data wave (batches 2531–2539:
 * sec_to_min_floor, min_to_sec, sec_to_hour_floor, hour_to_sec,
 * day_to_sec, sec_to_day_floor, week_to_sec, ms_add_sat_u,
 * ms_sub_sat_u). Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* or milestone_* symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2540_marker[] = "libcgj-batch2540";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2540_id(void)
{
	return 2540u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2540 — report this TU's graph batch number.
 *
 * Always returns 2540.
 */
uint32_t
gj_batch_id_2540(void)
{
	(void)NULL;
	return b2540_id();
}

/*
 * gj_graph_milestone_2540 — report this TU's graph milestone revision.
 *
 * Always returns 2540 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2540(void)
{
	return b2540_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2540(void)
    __attribute__((alias("gj_batch_id_2540")));

uint32_t __gj_graph_milestone_2540(void)
    __attribute__((alias("gj_graph_milestone_2540")));
