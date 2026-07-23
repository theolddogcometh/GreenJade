/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5643: installer format stage identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_install_stage_format_5643(void);
 *     - Returns 2 (soft format stage ordinal in the install pipeline).
 *   uint32_t gj_install_format_weight_pct_5643(void);
 *     - Returns 15 (soft weight of format stage in overall progress).
 *   uint32_t gj_batch_id_5643(void);
 *     - Returns 5643 (this TU's graph batch number).
 *   uint32_t __gj_install_stage_format_5643  (alias)
 *   uint32_t __gj_install_format_weight_pct_5643  (alias)
 *   uint32_t __gj_batch_id_5643  (alias)
 *   __libcgj_batch5643_marker = "libcgj-batch5643"
 *
 * Exclusive continuum CREATE-ONLY (5641-5650: installer stages —
 * partition/format/copy/bootloader/config progress integers). Unique
 * batch-suffixed symbols only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5643_marker[] = "libcgj-batch5643";

#define B5643_STAGE_FORMAT  2u
#define B5643_WEIGHT_PCT    15u
#define B5643_BATCH         5643u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5643_stage(void)
{
	return B5643_STAGE_FORMAT;
}

static uint32_t
b5643_weight(void)
{
	return B5643_WEIGHT_PCT;
}

static uint32_t
b5643_id(void)
{
	return B5643_BATCH;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_install_stage_format_5643 - soft format stage ordinal.
 *
 * Always returns 2. Product-path tag for filesystem format stage;
 * does not call mkfs or touch disks. No parent wires.
 */
uint32_t
gj_install_stage_format_5643(void)
{
	(void)NULL;
	return b5643_stage();
}

/*
 * gj_install_format_weight_pct_5643 - soft format stage weight.
 *
 * Always returns 15 (percent points of overall install progress).
 */
uint32_t
gj_install_format_weight_pct_5643(void)
{
	return b5643_weight();
}

/*
 * gj_batch_id_5643 - report this TU's graph batch number.
 *
 * Always returns 5643.
 */
uint32_t
gj_batch_id_5643(void)
{
	return b5643_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_install_stage_format_5643(void)
    __attribute__((alias("gj_install_stage_format_5643")));

uint32_t __gj_install_format_weight_pct_5643(void)
    __attribute__((alias("gj_install_format_weight_pct_5643")));

uint32_t __gj_batch_id_5643(void)
    __attribute__((alias("gj_batch_id_5643")));
