/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7032: sched_attr basic policy constants.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sched_attr_policy_normal_7032(void);
 *     - Returns 0 (soft SCHED_NORMAL / SCHED_OTHER).
 *   uint32_t gj_sched_attr_policy_fifo_7032(void);
 *     - Returns 1 (soft SCHED_FIFO).
 *   uint32_t gj_sched_attr_policy_rr_7032(void);
 *     - Returns 2 (soft SCHED_RR).
 *   uint32_t gj_sched_attr_policy_batch_7032(void);
 *     - Returns 3 (soft SCHED_BATCH).
 *   uint32_t gj_sched_attr_policy_basic_mask_7032(void);
 *     - Returns 0xF (soft bit tags for normal|fifo|rr|batch slots).
 *   uint32_t __gj_sched_attr_*_7032  (aliases)
 *   __libcgj_batch7032_marker = "libcgj-batch7032"
 *
 * Exclusive continuum CREATE-ONLY (7031-7040: sched_setattr policy stubs).
 * Unique gj_sched_attr_*_7032 surfaces only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7032_marker[] = "libcgj-batch7032";

/* Soft SCHED_* policy values (classic four). */
#define B7032_NORMAL  0u
#define B7032_FIFO    1u
#define B7032_RR      2u
#define B7032_BATCH   3u
/* Soft presence mask for the four basic policy slots (not policy ids). */
#define B7032_BASIC_MASK  0x0000000Fu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7032_normal(void)
{
	return B7032_NORMAL;
}

static uint32_t
b7032_fifo(void)
{
	return B7032_FIFO;
}

static uint32_t
b7032_rr(void)
{
	return B7032_RR;
}

static uint32_t
b7032_batch(void)
{
	return B7032_BATCH;
}

static uint32_t
b7032_basic_mask(void)
{
	return B7032_BASIC_MASK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sched_attr_policy_normal_7032 - soft SCHED_NORMAL / SCHED_OTHER.
 *
 * Always returns 0. Soft pure-data policy tag; no sched_setattr call.
 */
uint32_t
gj_sched_attr_policy_normal_7032(void)
{
	(void)NULL;
	return b7032_normal();
}

/*
 * gj_sched_attr_policy_fifo_7032 - soft SCHED_FIFO.
 *
 * Always returns 1.
 */
uint32_t
gj_sched_attr_policy_fifo_7032(void)
{
	return b7032_fifo();
}

/*
 * gj_sched_attr_policy_rr_7032 - soft SCHED_RR.
 *
 * Always returns 2.
 */
uint32_t
gj_sched_attr_policy_rr_7032(void)
{
	return b7032_rr();
}

/*
 * gj_sched_attr_policy_batch_7032 - soft SCHED_BATCH.
 *
 * Always returns 3.
 */
uint32_t
gj_sched_attr_policy_batch_7032(void)
{
	return b7032_batch();
}

/*
 * gj_sched_attr_policy_basic_mask_7032 - soft basic-policy slot mask.
 *
 * Always returns 0xF. Soft presence mask for the classic four policies.
 */
uint32_t
gj_sched_attr_policy_basic_mask_7032(void)
{
	return b7032_basic_mask();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_sched_attr_policy_normal_7032(void)
    __attribute__((alias("gj_sched_attr_policy_normal_7032")));

uint32_t __gj_sched_attr_policy_fifo_7032(void)
    __attribute__((alias("gj_sched_attr_policy_fifo_7032")));

uint32_t __gj_sched_attr_policy_rr_7032(void)
    __attribute__((alias("gj_sched_attr_policy_rr_7032")));

uint32_t __gj_sched_attr_policy_batch_7032(void)
    __attribute__((alias("gj_sched_attr_policy_batch_7032")));

uint32_t __gj_sched_attr_policy_basic_mask_7032(void)
    __attribute__((alias("gj_sched_attr_policy_basic_mask_7032")));
