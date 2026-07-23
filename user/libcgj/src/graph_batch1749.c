/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1749: throttle ratio in milli-units.
 *
 * Surface (unique symbols):
 *   uint64_t gj_throttle_ratio_milli(uint64_t used, uint64_t limit);
 *     — Return used * 1000 / limit (milli-ratio of consumption vs
 *       budget). limit == 0 → 0. Overflow-safe path when used * 1000
 *       would exceed UINT64_MAX. Result may exceed 1000 if used > limit.
 *   uint64_t __gj_throttle_ratio_milli  (alias)
 *   __libcgj_batch1749_marker = "libcgj-batch1749"
 *
 * Pure data for cgroup throttle / quota telemetry. Does NOT redefine
 * prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1749_marker[] = "libcgj-batch1749";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b1749_milli(uint64_t u64Used, uint64_t u64Limit)
{
	if (u64Limit == 0ull || u64Used == 0ull) {
		return 0ull;
	}
	if (u64Used > UINT64_MAX / 1000ull) {
		return (u64Used / u64Limit) * 1000ull +
		    ((u64Used % u64Limit) * 1000ull) / u64Limit;
	}
	return (u64Used * 1000ull) / u64Limit;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_throttle_ratio_milli — milli-ratio of used resource to limit.
 *
 * 1000 means fully consumed; above 1000 means over-budget samples.
 * Pure arithmetic for CFS throttle / memory pressure style accounting.
 */
uint64_t
gj_throttle_ratio_milli(uint64_t u64Used, uint64_t u64Limit)
{
	return b1749_milli(u64Used, u64Limit);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_throttle_ratio_milli(uint64_t u64Used, uint64_t u64Limit)
    __attribute__((alias("gj_throttle_ratio_milli")));
