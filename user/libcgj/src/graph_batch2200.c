/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2200: milestone 2200 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2200(void);
 *     - Returns the compile-time graph batch number for this TU (2200).
 *   uint32_t gj_graph_milestone_2200(void);
 *     - Returns the current graph milestone revision (2200).
 *   uint32_t __gj_batch_id_2200  (alias)
 *   uint32_t __gj_graph_milestone_2200  (alias)
 *   __libcgj_batch2200_marker = "libcgj-batch2200"
 *
 * Milestone for the exclusive product helpers wave (batches 2191-2199:
 * product_wave_id, libcgj_batch_cap_hint, shell_cmd_budget_hint,
 * export_floor_ok, milestone_is_hundred_p, milestone_is_thousand_p,
 * continuum_ok_hint, smoke_soft_skip_p, smoke_hard_fail_p). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2200_marker[] = "libcgj-batch2200";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2200_id(void)
{
	return 2200u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2200 - report this TU's graph batch number.
 *
 * Always returns 2200.
 */
uint32_t
gj_batch_id_2200(void)
{
	(void)NULL;
	return b2200_id();
}

/*
 * gj_graph_milestone_2200 - report this TU's graph milestone revision.
 *
 * Always returns 2200 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2200(void)
{
	return b2200_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2200(void)
    __attribute__((alias("gj_batch_id_2200")));

uint32_t __gj_graph_milestone_2200(void)
    __attribute__((alias("gj_graph_milestone_2200")));
