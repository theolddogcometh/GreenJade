/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2270: milestone 2270 batch identity (closes
 * post-2260 time exclusive wave 2261-2270).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2270(void);
 *     - Returns the compile-time graph batch number for this TU (2270).
 *   uint32_t gj_graph_milestone_2270(void);
 *     - Returns the current graph milestone revision (2270).
 *   uint32_t __gj_batch_id_2270  (alias)
 *   uint32_t __gj_graph_milestone_2270  (alias)
 *   __libcgj_batch2270_marker = "libcgj-batch2270"
 *
 * Milestone for the exclusive time helpers wave (batches 2261-2269:
 * timespec_add_ns, timespec_sub_ns, timespec_cmp_u, timeval_to_ms,
 * ms_to_timeval, duration_ms_add_sat, duration_ms_sub_sat,
 * rate_per_sec_u, eta_ms_hint). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* or milestone_* symbols -
 * avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2270_marker[] = "libcgj-batch2270";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2270_id(void)
{
	return 2270u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2270 - report this TU's graph batch number.
 *
 * Always returns 2270.
 */
uint32_t
gj_batch_id_2270(void)
{
	(void)NULL;
	return b2270_id();
}

/*
 * gj_graph_milestone_2270 - report this TU's graph milestone revision.
 *
 * Always returns 2270 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2270(void)
{
	return b2270_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2270(void)
    __attribute__((alias("gj_batch_id_2270")));

uint32_t __gj_graph_milestone_2270(void)
    __attribute__((alias("gj_graph_milestone_2270")));
