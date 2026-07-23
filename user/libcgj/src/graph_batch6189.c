/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6189: memory usage milli-ratio stub.
 *
 * Surface (unique symbols):
 *   uint64_t gj_cg_mem_usage_milli_6189(uint64_t used, uint64_t limit);
 *     - Return used * 1000 / limit (milli-ratio of memory.current vs
 *       memory.max). limit == 0 → 0. Overflow-safe path when
 *       used * 1000 would exceed UINT64_MAX. Result may exceed 1000 if
 *       used > limit.
 *   uint64_t __gj_cg_mem_usage_milli_6189  (alias)
 *   __libcgj_batch6189_marker = "libcgj-batch6189"
 *
 * Exclusive continuum CREATE-ONLY (6181-6190: container runtime cgroup
 * stubs). Unique gj_cg_mem_usage_milli_6189 surface only; no multi-def.
 * Distinct from gj_throttle_ratio_milli (batch1749). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6189_marker[] = "libcgj-batch6189";

#define B6189_MILLI  1000ull

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b6189_milli(uint64_t uUsed, uint64_t uLimit)
{
	if (uLimit == 0ull || uUsed == 0ull) {
		return 0ull;
	}
	if (uUsed > UINT64_MAX / B6189_MILLI) {
		return (uUsed / uLimit) * B6189_MILLI +
		    ((uUsed % uLimit) * B6189_MILLI) / uLimit;
	}
	return (uUsed * B6189_MILLI) / uLimit;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cg_mem_usage_milli_6189 - milli-ratio of memory used to limit.
 *
 * 1000 means fully consumed; above 1000 means over-budget samples.
 * Pure arithmetic for container runtime memory pressure telemetry.
 * Does not call libc. No parent wires.
 */
uint64_t
gj_cg_mem_usage_milli_6189(uint64_t uUsed, uint64_t uLimit)
{
	(void)NULL;
	return b6189_milli(uUsed, uLimit);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_cg_mem_usage_milli_6189(uint64_t uUsed, uint64_t uLimit)
    __attribute__((alias("gj_cg_mem_usage_milli_6189")));
