/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2020: milestone 2020 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2020(void);
 *     - Returns the compile-time graph batch number for this TU (2020).
 *   uint32_t gj_graph_milestone_2020(void);
 *     - Returns the current graph milestone revision (2020).
 *   uint32_t __gj_batch_id_2020  (alias)
 *   uint32_t __gj_graph_milestone_2020  (alias)
 *   __libcgj_batch2020_marker = "libcgj-batch2020"
 *
 * Milestone for the post-2000 bit-ops exclusive wave (batches 2011-2019:
 * u64 set/clear/toggle/test, u64 mask low/high n, u32 set/clear/test).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * or milestone_* symbols - avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2020_marker[] = "libcgj-batch2020";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2020_id(void)
{
	return 2020u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2020 - report this TU's graph batch number.
 *
 * Always returns 2020.
 */
uint32_t
gj_batch_id_2020(void)
{
	(void)NULL;
	return b2020_id();
}

/*
 * gj_graph_milestone_2020 - report this TU's graph milestone revision.
 *
 * Always returns 2020 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2020(void)
{
	return b2020_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2020(void)
    __attribute__((alias("gj_batch_id_2020")));

uint32_t __gj_graph_milestone_2020(void)
    __attribute__((alias("gj_graph_milestone_2020")));
