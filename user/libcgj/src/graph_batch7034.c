/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7034: sched_attr sched_flags constants.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sched_attr_flag_reset_on_fork_7034(void);
 *     - Returns 0x01 (soft SCHED_FLAG_RESET_ON_FORK).
 *   uint32_t gj_sched_attr_flag_reclaim_7034(void);
 *     - Returns 0x02 (soft SCHED_FLAG_RECLAIM).
 *   uint32_t gj_sched_attr_flag_dl_overrun_7034(void);
 *     - Returns 0x04 (soft SCHED_FLAG_DL_OVERRUN).
 *   uint32_t gj_sched_attr_flag_keep_policy_7034(void);
 *     - Returns 0x08 (soft SCHED_FLAG_KEEP_POLICY).
 *   uint32_t gj_sched_attr_flag_keep_params_7034(void);
 *     - Returns 0x10 (soft SCHED_FLAG_KEEP_PARAMS).
 *   uint32_t gj_sched_attr_flag_util_clamp_7034(void);
 *     - Returns 0x60 (soft SCHED_FLAG_UTIL_CLAMP min|max).
 *   uint32_t __gj_sched_attr_*_7034  (aliases)
 *   __libcgj_batch7034_marker = "libcgj-batch7034"
 *
 * Exclusive continuum CREATE-ONLY (7031-7040: sched_setattr policy stubs).
 * Unique gj_sched_attr_*_7034 surfaces only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7034_marker[] = "libcgj-batch7034";

/* Soft SCHED_FLAG_* bits (mirror linux/sched.h). */
#define B7034_RESET_ON_FORK  0x01u
#define B7034_RECLAIM        0x02u
#define B7034_DL_OVERRUN     0x04u
#define B7034_KEEP_POLICY    0x08u
#define B7034_KEEP_PARAMS    0x10u
#define B7034_UTIL_CLAMP_MIN 0x20u
#define B7034_UTIL_CLAMP_MAX 0x40u
#define B7034_UTIL_CLAMP     (B7034_UTIL_CLAMP_MIN | B7034_UTIL_CLAMP_MAX)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7034_reset_on_fork(void)
{
	return B7034_RESET_ON_FORK;
}

static uint32_t
b7034_reclaim(void)
{
	return B7034_RECLAIM;
}

static uint32_t
b7034_dl_overrun(void)
{
	return B7034_DL_OVERRUN;
}

static uint32_t
b7034_keep_policy(void)
{
	return B7034_KEEP_POLICY;
}

static uint32_t
b7034_keep_params(void)
{
	return B7034_KEEP_PARAMS;
}

static uint32_t
b7034_util_clamp(void)
{
	return B7034_UTIL_CLAMP;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sched_attr_flag_reset_on_fork_7034 - soft SCHED_FLAG_RESET_ON_FORK.
 *
 * Always returns 0x01. Soft pure-data flag tag; no sched_setattr call.
 */
uint32_t
gj_sched_attr_flag_reset_on_fork_7034(void)
{
	(void)NULL;
	return b7034_reset_on_fork();
}

/*
 * gj_sched_attr_flag_reclaim_7034 - soft SCHED_FLAG_RECLAIM.
 *
 * Always returns 0x02.
 */
uint32_t
gj_sched_attr_flag_reclaim_7034(void)
{
	return b7034_reclaim();
}

/*
 * gj_sched_attr_flag_dl_overrun_7034 - soft SCHED_FLAG_DL_OVERRUN.
 *
 * Always returns 0x04.
 */
uint32_t
gj_sched_attr_flag_dl_overrun_7034(void)
{
	return b7034_dl_overrun();
}

/*
 * gj_sched_attr_flag_keep_policy_7034 - soft SCHED_FLAG_KEEP_POLICY.
 *
 * Always returns 0x08.
 */
uint32_t
gj_sched_attr_flag_keep_policy_7034(void)
{
	return b7034_keep_policy();
}

/*
 * gj_sched_attr_flag_keep_params_7034 - soft SCHED_FLAG_KEEP_PARAMS.
 *
 * Always returns 0x10.
 */
uint32_t
gj_sched_attr_flag_keep_params_7034(void)
{
	return b7034_keep_params();
}

/*
 * gj_sched_attr_flag_util_clamp_7034 - soft SCHED_FLAG_UTIL_CLAMP.
 *
 * Always returns 0x60 (UTIL_CLAMP_MIN | UTIL_CLAMP_MAX).
 */
uint32_t
gj_sched_attr_flag_util_clamp_7034(void)
{
	return b7034_util_clamp();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_sched_attr_flag_reset_on_fork_7034(void)
    __attribute__((alias("gj_sched_attr_flag_reset_on_fork_7034")));

uint32_t __gj_sched_attr_flag_reclaim_7034(void)
    __attribute__((alias("gj_sched_attr_flag_reclaim_7034")));

uint32_t __gj_sched_attr_flag_dl_overrun_7034(void)
    __attribute__((alias("gj_sched_attr_flag_dl_overrun_7034")));

uint32_t __gj_sched_attr_flag_keep_policy_7034(void)
    __attribute__((alias("gj_sched_attr_flag_keep_policy_7034")));

uint32_t __gj_sched_attr_flag_keep_params_7034(void)
    __attribute__((alias("gj_sched_attr_flag_keep_params_7034")));

uint32_t __gj_sched_attr_flag_util_clamp_7034(void)
    __attribute__((alias("gj_sched_attr_flag_util_clamp_7034")));
