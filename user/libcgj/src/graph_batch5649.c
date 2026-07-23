/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5649: installer config stage identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_install_stage_config_5649(void);
 *     - Returns 5 (soft config stage ordinal in the install pipeline).
 *   uint32_t gj_install_config_weight_pct_5649(void);
 *     - Returns 15 (soft weight of config stage in overall progress).
 *   uint32_t gj_install_stage_count_5649(void);
 *     - Returns 5 (total installer stages: partition..config).
 *   uint32_t gj_batch_id_5649(void);
 *     - Returns 5649 (this TU's graph batch number).
 *   uint32_t __gj_install_stage_config_5649  (alias)
 *   uint32_t __gj_install_config_weight_pct_5649  (alias)
 *   uint32_t __gj_install_stage_count_5649  (alias)
 *   uint32_t __gj_batch_id_5649  (alias)
 *   __libcgj_batch5649_marker = "libcgj-batch5649"
 *
 * Exclusive continuum CREATE-ONLY (5641-5650: installer stages —
 * partition/format/copy/bootloader/config progress integers). Unique
 * batch-suffixed symbols only; no multi-def. Weights sum:
 * 15+15+40+15+15 = 100. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5649_marker[] = "libcgj-batch5649";

#define B5649_STAGE_CONFIG  5u
#define B5649_WEIGHT_PCT    15u
#define B5649_STAGE_COUNT   5u
#define B5649_BATCH         5649u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5649_stage(void)
{
	return B5649_STAGE_CONFIG;
}

static uint32_t
b5649_weight(void)
{
	return B5649_WEIGHT_PCT;
}

static uint32_t
b5649_count(void)
{
	return B5649_STAGE_COUNT;
}

static uint32_t
b5649_id(void)
{
	return B5649_BATCH;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_install_stage_config_5649 - soft config stage ordinal.
 *
 * Always returns 5. Product-path tag for post-install config stage
 * (locale, user, fstab stubs); does not write files. No parent wires.
 */
uint32_t
gj_install_stage_config_5649(void)
{
	(void)NULL;
	return b5649_stage();
}

/*
 * gj_install_config_weight_pct_5649 - soft config stage weight.
 *
 * Always returns 15 (percent points of overall install progress).
 */
uint32_t
gj_install_config_weight_pct_5649(void)
{
	return b5649_weight();
}

/*
 * gj_install_stage_count_5649 - total installer pipeline stage count.
 *
 * Always returns 5 (partition, format, copy, bootloader, config).
 */
uint32_t
gj_install_stage_count_5649(void)
{
	return b5649_count();
}

/*
 * gj_batch_id_5649 - report this TU's graph batch number.
 *
 * Always returns 5649.
 */
uint32_t
gj_batch_id_5649(void)
{
	return b5649_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_install_stage_config_5649(void)
    __attribute__((alias("gj_install_stage_config_5649")));

uint32_t __gj_install_config_weight_pct_5649(void)
    __attribute__((alias("gj_install_config_weight_pct_5649")));

uint32_t __gj_install_stage_count_5649(void)
    __attribute__((alias("gj_install_stage_count_5649")));

uint32_t __gj_batch_id_5649(void)
    __attribute__((alias("gj_batch_id_5649")));
