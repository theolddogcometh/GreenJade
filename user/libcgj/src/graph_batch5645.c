/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5645: installer copy stage identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_install_stage_copy_5645(void);
 *     - Returns 3 (soft copy stage ordinal in the install pipeline).
 *   uint32_t gj_install_copy_weight_pct_5645(void);
 *     - Returns 40 (soft weight of copy stage — bulk of install time).
 *   uint32_t gj_batch_id_5645(void);
 *     - Returns 5645 (this TU's graph batch number).
 *   uint32_t __gj_install_stage_copy_5645  (alias)
 *   uint32_t __gj_install_copy_weight_pct_5645  (alias)
 *   uint32_t __gj_batch_id_5645  (alias)
 *   __libcgj_batch5645_marker = "libcgj-batch5645"
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

const char __libcgj_batch5645_marker[] = "libcgj-batch5645";

#define B5645_STAGE_COPY  3u
/* Copy dominates install wall-clock; 40 percent points of overall. */
#define B5645_WEIGHT_PCT  40u
#define B5645_BATCH       5645u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5645_stage(void)
{
	return B5645_STAGE_COPY;
}

static uint32_t
b5645_weight(void)
{
	return B5645_WEIGHT_PCT;
}

static uint32_t
b5645_id(void)
{
	return B5645_BATCH;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_install_stage_copy_5645 - soft copy stage ordinal.
 *
 * Always returns 3. Product-path tag for rootfs/payload copy stage;
 * does not open filesystems. No parent wires.
 */
uint32_t
gj_install_stage_copy_5645(void)
{
	(void)NULL;
	return b5645_stage();
}

/*
 * gj_install_copy_weight_pct_5645 - soft copy stage weight.
 *
 * Always returns 40 (percent points of overall install progress).
 */
uint32_t
gj_install_copy_weight_pct_5645(void)
{
	return b5645_weight();
}

/*
 * gj_batch_id_5645 - report this TU's graph batch number.
 *
 * Always returns 5645.
 */
uint32_t
gj_batch_id_5645(void)
{
	return b5645_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_install_stage_copy_5645(void)
    __attribute__((alias("gj_install_stage_copy_5645")));

uint32_t __gj_install_copy_weight_pct_5645(void)
    __attribute__((alias("gj_install_copy_weight_pct_5645")));

uint32_t __gj_batch_id_5645(void)
    __attribute__((alias("gj_batch_id_5645")));
