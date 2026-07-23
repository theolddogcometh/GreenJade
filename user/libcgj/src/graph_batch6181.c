/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6181: cgroup cpu.max period validity stub.
 *
 * Surface (unique symbols):
 *   int gj_cg_cpu_max_period_ok_6181(uint64_t period_us);
 *     - Return 1 if period_us is in the classic CFS / cgroup v2 cpu.max
 *       period window [1000, 1000000] microseconds (1 ms .. 1 s), else 0.
 *   int __gj_cg_cpu_max_period_ok_6181  (alias)
 *   __libcgj_batch6181_marker = "libcgj-batch6181"
 *
 * Exclusive continuum CREATE-ONLY (6181-6190: container runtime cgroup
 * stubs — cpu_max_period_ok, millicores_to_quota_us, mem_max_clamp,
 * pids_headroom, weight_clamp, cpuset_mask_pop, freezer_state_ok,
 * io_max_bps_clamp, mem_usage_milli, continuum + batch_id_6190).
 * Unique gj_cg_cpu_max_period_ok_6181 surface only; no multi-def.
 * Distinct from gj_quota_period_ok (batch1748). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6181_marker[] = "libcgj-batch6181";

/* CFS / cgroup v2 cpu.max period window (microseconds). */
#define B6181_PERIOD_MIN_US  1000ull
#define B6181_PERIOD_MAX_US  1000000ull

/* ---- freestanding helpers ---------------------------------------------- */

static int
b6181_period_ok(uint64_t uPeriodUs)
{
	if (uPeriodUs < B6181_PERIOD_MIN_US) {
		return 0;
	}
	if (uPeriodUs > B6181_PERIOD_MAX_US) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cg_cpu_max_period_ok_6181 - 1 if period_us is a valid cpu.max period.
 *
 * Screens parsed cpu.max period fields for container runtime cgroup
 * policy math. Pure predicate; no cgroupfs, no sched_entity.
 * Does not call libc. No parent wires.
 */
int
gj_cg_cpu_max_period_ok_6181(uint64_t uPeriodUs)
{
	(void)NULL;
	return b6181_period_ok(uPeriodUs);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_cg_cpu_max_period_ok_6181(uint64_t uPeriodUs)
    __attribute__((alias("gj_cg_cpu_max_period_ok_6181")));
