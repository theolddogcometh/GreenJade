/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1617: CFS virtual slice hint.
 *
 * Surface (unique symbols):
 *   uint64_t gj_cfs_vslice_hint(uint64_t period, uint32_t weight,
 *                               uint32_t total_weight);
 *     — period * weight / total_weight (fair share of a latency period).
 *       total_weight == 0 → 0. weight == 0 → 0. Saturates the product
 *       path when period * weight would overflow uint64.
 *   uint64_t __gj_cfs_vslice_hint  (alias)
 *   __libcgj_batch1617_marker = "libcgj-batch1617"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1617_marker[] = "libcgj-batch1617";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cfs_vslice_hint — approximate CFS sched_slice for pure data use.
 *
 * Models the classic latency-period share without rq/cfs_rq objects.
 * Callers supply period (e.g. target latency ticks) and load weights.
 */
uint64_t
gj_cfs_vslice_hint(uint64_t u64Period, uint32_t u32Weight, uint32_t u32Total)
{
	uint64_t u64W;
	uint64_t u64T;

	if (u32Total == 0u || u32Weight == 0u || u64Period == 0ull) {
		return 0ull;
	}

	u64W = (uint64_t)u32Weight;
	u64T = (uint64_t)u32Total;

	if (u64Period > UINT64_MAX / u64W) {
		/* Divide first to avoid overflow; small remainder term. */
		return (u64Period / u64T) * u64W +
		    ((u64Period % u64T) * u64W) / u64T;
	}
	return (u64Period * u64W) / u64T;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_cfs_vslice_hint(uint64_t u64Period, uint32_t u32Weight,
    uint32_t u32Total)
    __attribute__((alias("gj_cfs_vslice_hint")));
