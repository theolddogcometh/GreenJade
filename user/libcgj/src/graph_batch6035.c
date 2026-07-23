/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6035: GPU memory heap budget ok predicates.
 *
 * Surface (unique symbols):
 *   uint32_t gj_gpu_heap_budget_ok_6035(uint32_t usable, uint32_t min_mib);
 *     - Return 1 when usable >= min_mib, else 0.
 *   uint32_t gj_gpu_heap_budget_headroom_ok_6035(uint32_t free_mib,
 *                                               uint32_t min_free);
 *     - Return 1 when free_mib >= min_free, else 0.
 *   uint32_t gj_gpu_heap_budget_pair_ok_6035(uint32_t usable, uint32_t min_u,
 *                                           uint32_t free_mib, uint32_t min_f);
 *     - Return 1 when both usable and free thresholds pass, else 0.
 *   uint32_t gj_gpu_heap_budget_ok_ready_6035(void);
 *     - Returns 1 (soft ok-predicate path ready lamp).
 *   uint32_t __gj_gpu_heap_budget_ok_6035  (alias)
 *   uint32_t __gj_gpu_heap_budget_headroom_ok_6035  (alias)
 *   uint32_t __gj_gpu_heap_budget_pair_ok_6035  (alias)
 *   uint32_t __gj_gpu_heap_budget_ok_ready_6035  (alias)
 *   __libcgj_batch6035_marker = "libcgj-batch6035"
 *
 * Exclusive continuum CREATE-ONLY (6031-6040: GPU memory heap budget
 * stubs). Ok predicates; distinct from gj_vram_budget_ok_5605. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6035_marker[] = "libcgj-batch6035";

#define B6035_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6035_ok(uint32_t u32Usable, uint32_t u32Min)
{
	return (u32Usable >= u32Min) ? 1u : 0u;
}

static uint32_t
b6035_headroom_ok(uint32_t u32Free, uint32_t u32MinFree)
{
	return (u32Free >= u32MinFree) ? 1u : 0u;
}

static uint32_t
b6035_pair_ok(uint32_t u32Usable, uint32_t u32MinU,
              uint32_t u32Free, uint32_t u32MinF)
{
	if (u32Usable < u32MinU) {
		return 0u;
	}
	if (u32Free < u32MinF) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gpu_heap_budget_ok_6035 - soft check usable against minimum.
 *
 * usable:  usable MiB
 * min_mib: minimum required MiB
 *
 * Returns 1 when usable >= min_mib, else 0.
 */
uint32_t
gj_gpu_heap_budget_ok_6035(uint32_t u32Usable, uint32_t u32MinMib)
{
	(void)NULL;
	return b6035_ok(u32Usable, u32MinMib);
}

/*
 * gj_gpu_heap_budget_headroom_ok_6035 - soft free-headroom check.
 *
 * free_mib: current free MiB
 * min_free: minimum free MiB required
 *
 * Returns 1 when free_mib >= min_free, else 0.
 */
uint32_t
gj_gpu_heap_budget_headroom_ok_6035(uint32_t u32FreeMib, uint32_t u32MinFree)
{
	return b6035_headroom_ok(u32FreeMib, u32MinFree);
}

/*
 * gj_gpu_heap_budget_pair_ok_6035 - soft dual usable+free check.
 *
 * usable/min_u: usable threshold pair
 * free_mib/min_f: free headroom threshold pair
 *
 * Returns 1 when both pass, else 0.
 */
uint32_t
gj_gpu_heap_budget_pair_ok_6035(uint32_t u32Usable, uint32_t u32MinU,
                                uint32_t u32FreeMib, uint32_t u32MinF)
{
	return b6035_pair_ok(u32Usable, u32MinU, u32FreeMib, u32MinF);
}

/*
 * gj_gpu_heap_budget_ok_ready_6035 - soft ok-predicate ready lamp.
 *
 * Always returns 1.
 */
uint32_t
gj_gpu_heap_budget_ok_ready_6035(void)
{
	return B6035_READY;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_gpu_heap_budget_ok_6035(uint32_t u32Usable, uint32_t u32MinMib)
    __attribute__((alias("gj_gpu_heap_budget_ok_6035")));

uint32_t __gj_gpu_heap_budget_headroom_ok_6035(uint32_t u32FreeMib,
                                               uint32_t u32MinFree)
    __attribute__((alias("gj_gpu_heap_budget_headroom_ok_6035")));

uint32_t __gj_gpu_heap_budget_pair_ok_6035(uint32_t u32Usable, uint32_t u32MinU,
                                           uint32_t u32FreeMib, uint32_t u32MinF)
    __attribute__((alias("gj_gpu_heap_budget_pair_ok_6035")));

uint32_t __gj_gpu_heap_budget_ok_ready_6035(void)
    __attribute__((alias("gj_gpu_heap_budget_ok_ready_6035")));
