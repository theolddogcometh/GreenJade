/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5030: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5030(void);
 *     - Returns the compile-time graph batch number for this TU (5030).
 *   uint32_t __gj_batch_id_5030  (alias)
 *   __libcgj_batch5030_marker = "libcgj-batch5030"
 *
 * Exclusive continuum CREATE-ONLY (5021-5030: sched_nice_clamp_s,
 * sched_prio_to_weight_u, sched_timeslice_u, sched_vruntime_add_u,
 * sched_vruntime_diff_u, sched_is_idle_u, sched_is_running_u,
 * sched_cpu_mask_bit_u, sched_cpu_mask_test_u, batch_id_5030).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5030_marker[] = "libcgj-batch5030";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5030_id(void)
{
	return 5030u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_5030 - report this TU's graph batch number.
 *
 * Always returns 5030. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_5030(void)
{
	(void)NULL;
	return b5030_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_5030(void)
    __attribute__((alias("gj_batch_id_5030")));
