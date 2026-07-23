/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2070: milestone 2070 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2070(void);
 *     - Returns the compile-time graph batch number for this TU (2070).
 *   uint32_t gj_graph_milestone_2070(void);
 *     - Returns the current graph milestone revision (2070).
 *   uint32_t __gj_batch_id_2070  (alias)
 *   uint32_t __gj_graph_milestone_2070  (alias)
 *   __libcgj_batch2070_marker = "libcgj-batch2070"
 *
 * Milestone for the array u32 exclusive wave (batches 2061-2069:
 * sum_n, xor_n, or_n, and_n, min_n, max_n, count_eq_n, count_lt_n,
 * any_eq_n). Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* or milestone_* symbols - avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2070_marker[] = "libcgj-batch2070";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2070_id(void)
{
	return 2070u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2070 - report this TU's graph batch number.
 *
 * Always returns 2070.
 */
uint32_t
gj_batch_id_2070(void)
{
	(void)NULL;
	return b2070_id();
}

/*
 * gj_graph_milestone_2070 - report this TU's graph milestone revision.
 *
 * Always returns 2070 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2070(void)
{
	return b2070_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2070(void)
    __attribute__((alias("gj_batch_id_2070")));

uint32_t __gj_graph_milestone_2070(void)
    __attribute__((alias("gj_graph_milestone_2070")));
