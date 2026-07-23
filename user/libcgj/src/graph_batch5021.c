/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5021: scheduler nice clamp via u32 map.
 *
 * Surface (unique symbols):
 *   int32_t gj_sched_nice_clamp_s(int32_t n);
 *     - Clamp n into the portable nice range [-20, 19] by mapping to
 *       an unsigned index 0..39, saturating that index, then mapping
 *       back: result = (int32_t)u - 20.
 *   int32_t __gj_sched_nice_clamp_s  (alias)
 *   __libcgj_batch5021_marker = "libcgj-batch5021"
 *
 * Exclusive continuum CREATE-ONLY (5021-5030: sched_nice_clamp_s,
 * sched_prio_to_weight_u, sched_timeslice_u, sched_vruntime_add_u,
 * sched_vruntime_diff_u, sched_is_idle_u, sched_is_running_u,
 * sched_cpu_mask_bit_u, sched_cpu_mask_test_u, batch_id_5030).
 * Distinct from gj_nice_clamp (batch1664) and gj_timeslice_from_nice
 * (batch1615) — unique gj_sched_nice_clamp_s surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5021_marker[] = "libcgj-batch5021";

/* Nice range as unsigned map: nice_u = nice + 20 in 0..39. */
#define B5021_NICE_U_MIN  0u
#define B5021_NICE_U_MAX  39u
#define B5021_NICE_SHIFT  20

/* ---- freestanding helpers ---------------------------------------------- */

static int32_t
b5021_nice_clamp(int32_t i32N)
{
	uint32_t u32Map;

	/*
	 * Map to u32 0..39 then back. Values at or below -20 become
	 * index 0; values at or above 19 become index 39.
	 */
	if (i32N <= -B5021_NICE_SHIFT) {
		u32Map = B5021_NICE_U_MIN;
	} else if (i32N >= 19) {
		u32Map = B5021_NICE_U_MAX;
	} else {
		u32Map = (uint32_t)(i32N + B5021_NICE_SHIFT);
	}

	return (int32_t)u32Map - B5021_NICE_SHIFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sched_nice_clamp_s - clamp a nice value into [-20, 19].
 *
 * n: candidate nice (any int32_t)
 *
 * Returns the nearest value in [-20, 19]. Mapping uses an intermediate
 * unsigned 0..39 index so the clamp is free of multi-way signed
 * edge cases. Self-contained; no parent wires.
 */
int32_t
gj_sched_nice_clamp_s(int32_t i32N)
{
	(void)NULL;
	return b5021_nice_clamp(i32N);
}

/* ---- underscored alias ------------------------------------------------- */

int32_t __gj_sched_nice_clamp_s(int32_t i32N)
    __attribute__((alias("gj_sched_nice_clamp_s")));
