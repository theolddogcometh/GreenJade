/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5641: installer partition stage identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_install_stage_partition_5641(void);
 *     - Returns 1 (soft partition stage ordinal in the install pipeline).
 *   uint32_t gj_install_partition_weight_pct_5641(void);
 *     - Returns 15 (soft weight of partition stage in overall progress).
 *   uint32_t gj_batch_id_5641(void);
 *     - Returns 5641 (this TU's graph batch number).
 *   uint32_t __gj_install_stage_partition_5641  (alias)
 *   uint32_t __gj_install_partition_weight_pct_5641  (alias)
 *   uint32_t __gj_batch_id_5641  (alias)
 *   __libcgj_batch5641_marker = "libcgj-batch5641"
 *
 * Exclusive continuum CREATE-ONLY (5641-5650: installer stages —
 * partition/format/copy/bootloader/config progress integers). Unique
 * batch-suffixed symbols only; no multi-def of bare install_stage names.
 * Distinct from gj_install_stage_ok (batch1994). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5641_marker[] = "libcgj-batch5641";

/* Stage ordinal: 1=partition, 2=format, 3=copy, 4=bootloader, 5=config. */
#define B5641_STAGE_PARTITION  1u
/* Soft weight of partition stage within overall install (percent points). */
#define B5641_WEIGHT_PCT       15u
#define B5641_BATCH            5641u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5641_stage(void)
{
	return B5641_STAGE_PARTITION;
}

static uint32_t
b5641_weight(void)
{
	return B5641_WEIGHT_PCT;
}

static uint32_t
b5641_id(void)
{
	return B5641_BATCH;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_install_stage_partition_5641 - soft partition stage ordinal.
 *
 * Always returns 1. Compile-time product tag for the install pipeline
 * first stage; does not touch disks. No parent wires.
 */
uint32_t
gj_install_stage_partition_5641(void)
{
	(void)NULL;
	return b5641_stage();
}

/*
 * gj_install_partition_weight_pct_5641 - soft partition stage weight.
 *
 * Always returns 15 (percent points of overall install progress).
 */
uint32_t
gj_install_partition_weight_pct_5641(void)
{
	return b5641_weight();
}

/*
 * gj_batch_id_5641 - report this TU's graph batch number.
 *
 * Always returns 5641.
 */
uint32_t
gj_batch_id_5641(void)
{
	return b5641_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_install_stage_partition_5641(void)
    __attribute__((alias("gj_install_stage_partition_5641")));

uint32_t __gj_install_partition_weight_pct_5641(void)
    __attribute__((alias("gj_install_partition_weight_pct_5641")));

uint32_t __gj_batch_id_5641(void)
    __attribute__((alias("gj_batch_id_5641")));
