/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6033: GPU memory heap budget minimums.
 *
 * Surface (unique symbols):
 *   uint32_t gj_gpu_heap_budget_min_mib_6033(void);
 *     - Returns 512 (soft minimum acceptable device-local budget MiB).
 *   uint32_t gj_gpu_heap_budget_host_min_mib_6033(void);
 *     - Returns 128 (soft minimum host-visible budget MiB).
 *   uint32_t gj_gpu_heap_budget_min_free_mib_6033(void);
 *     - Returns 64 (soft minimum free headroom MiB before pressure).
 *   uint32_t gj_gpu_heap_budget_min_score_6033(void);
 *     - Returns 6033 (wave-tagged min-budget score).
 *   uint32_t __gj_gpu_heap_budget_min_mib_6033  (alias)
 *   uint32_t __gj_gpu_heap_budget_host_min_mib_6033  (alias)
 *   uint32_t __gj_gpu_heap_budget_min_free_mib_6033  (alias)
 *   uint32_t __gj_gpu_heap_budget_min_score_6033  (alias)
 *   __libcgj_batch6033_marker = "libcgj-batch6033"
 *
 * Exclusive continuum CREATE-ONLY (6031-6040: GPU memory heap budget
 * stubs). Soft floor constants; no device probe. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6033_marker[] = "libcgj-batch6033";

#define B6033_MIN_MIB       512u
#define B6033_HOST_MIN_MIB  128u
#define B6033_MIN_FREE_MIB  64u
#define B6033_SCORE         6033u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6033_min(void)
{
	return B6033_MIN_MIB;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gpu_heap_budget_min_mib_6033 - soft minimum device-local budget.
 *
 * Always returns 512 MiB.
 */
uint32_t
gj_gpu_heap_budget_min_mib_6033(void)
{
	(void)NULL;
	return b6033_min();
}

/*
 * gj_gpu_heap_budget_host_min_mib_6033 - soft minimum host-visible budget.
 *
 * Always returns 128 MiB.
 */
uint32_t
gj_gpu_heap_budget_host_min_mib_6033(void)
{
	return B6033_HOST_MIN_MIB;
}

/*
 * gj_gpu_heap_budget_min_free_mib_6033 - soft minimum free headroom.
 *
 * Always returns 64 MiB.
 */
uint32_t
gj_gpu_heap_budget_min_free_mib_6033(void)
{
	return B6033_MIN_FREE_MIB;
}

/*
 * gj_gpu_heap_budget_min_score_6033 - wave-tagged min-budget score.
 *
 * Always returns 6033.
 */
uint32_t
gj_gpu_heap_budget_min_score_6033(void)
{
	return B6033_SCORE;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_gpu_heap_budget_min_mib_6033(void)
    __attribute__((alias("gj_gpu_heap_budget_min_mib_6033")));

uint32_t __gj_gpu_heap_budget_host_min_mib_6033(void)
    __attribute__((alias("gj_gpu_heap_budget_host_min_mib_6033")));

uint32_t __gj_gpu_heap_budget_min_free_mib_6033(void)
    __attribute__((alias("gj_gpu_heap_budget_min_free_mib_6033")));

uint32_t __gj_gpu_heap_budget_min_score_6033(void)
    __attribute__((alias("gj_gpu_heap_budget_min_score_6033")));
