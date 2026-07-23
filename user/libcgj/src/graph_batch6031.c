/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6031: GPU memory heap budget totals.
 *
 * Surface (unique symbols):
 *   uint32_t gj_gpu_heap_budget_total_mib_6031(void);
 *     - Returns 2048 (soft default GPU device-local heap budget MiB).
 *   uint32_t gj_gpu_heap_budget_host_total_mib_6031(void);
 *     - Returns 1024 (soft default host-visible heap budget MiB).
 *   uint32_t gj_gpu_heap_budget_score_6031(void);
 *     - Returns 6031 (wave-tagged heap budget score).
 *   uint32_t gj_batch_id_6031(void);
 *     - Returns 6031 (this TU's graph batch number).
 *   uint32_t __gj_gpu_heap_budget_total_mib_6031  (alias)
 *   uint32_t __gj_gpu_heap_budget_host_total_mib_6031  (alias)
 *   uint32_t __gj_gpu_heap_budget_score_6031  (alias)
 *   uint32_t __gj_batch_id_6031  (alias)
 *   __libcgj_batch6031_marker = "libcgj-batch6031"
 *
 * Exclusive continuum CREATE-ONLY (6031-6040: GPU memory heap budget
 * stubs). Soft product-path constants; no real device probe. Distinct
 * from gj_vram_budget_*_5604. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6031_marker[] = "libcgj-batch6031";

#define B6031_TOTAL_MIB       2048u
#define B6031_HOST_TOTAL_MIB  1024u
#define B6031_SCORE           6031u
#define B6031_BATCH           6031u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6031_total(void)
{
	return B6031_TOTAL_MIB;
}

static uint32_t
b6031_id(void)
{
	return B6031_BATCH;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gpu_heap_budget_total_mib_6031 - soft device-local heap budget total.
 *
 * Always returns 2048 MiB. Product-path constant; not a hw probe.
 */
uint32_t
gj_gpu_heap_budget_total_mib_6031(void)
{
	(void)NULL;
	return b6031_total();
}

/*
 * gj_gpu_heap_budget_host_total_mib_6031 - soft host-visible heap total.
 *
 * Always returns 1024 MiB.
 */
uint32_t
gj_gpu_heap_budget_host_total_mib_6031(void)
{
	return B6031_HOST_TOTAL_MIB;
}

/*
 * gj_gpu_heap_budget_score_6031 - wave-tagged heap budget score.
 *
 * Always returns 6031.
 */
uint32_t
gj_gpu_heap_budget_score_6031(void)
{
	return B6031_SCORE;
}

/*
 * gj_batch_id_6031 - report this TU's graph batch number.
 *
 * Always returns 6031.
 */
uint32_t
gj_batch_id_6031(void)
{
	return b6031_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_gpu_heap_budget_total_mib_6031(void)
    __attribute__((alias("gj_gpu_heap_budget_total_mib_6031")));

uint32_t __gj_gpu_heap_budget_host_total_mib_6031(void)
    __attribute__((alias("gj_gpu_heap_budget_host_total_mib_6031")));

uint32_t __gj_gpu_heap_budget_score_6031(void)
    __attribute__((alias("gj_gpu_heap_budget_score_6031")));

uint32_t __gj_batch_id_6031(void)
    __attribute__((alias("gj_batch_id_6031")));
