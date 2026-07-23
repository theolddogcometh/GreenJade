/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7033: sched_attr extended policy constants.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sched_attr_policy_idle_7033(void);
 *     - Returns 5 (soft SCHED_IDLE).
 *   uint32_t gj_sched_attr_policy_deadline_7033(void);
 *     - Returns 6 (soft SCHED_DEADLINE).
 *   uint32_t gj_sched_attr_policy_is_rt_7033(uint32_t policy);
 *     - Return 1 if policy is FIFO or RR; else 0.
 *   uint32_t gj_sched_attr_policy_is_dl_7033(uint32_t policy);
 *     - Return 1 if policy is DEADLINE; else 0.
 *   uint32_t __gj_sched_attr_*_7033  (aliases)
 *   __libcgj_batch7033_marker = "libcgj-batch7033"
 *
 * Exclusive continuum CREATE-ONLY (7031-7040: sched_setattr policy stubs).
 * Unique gj_sched_attr_*_7033 surfaces only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7033_marker[] = "libcgj-batch7033";

/* Soft SCHED_IDLE / SCHED_DEADLINE (gap 4 is reserved ISO). */
#define B7033_IDLE      5u
#define B7033_DEADLINE  6u
#define B7033_FIFO      1u
#define B7033_RR        2u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7033_idle(void)
{
	return B7033_IDLE;
}

static uint32_t
b7033_deadline(void)
{
	return B7033_DEADLINE;
}

static uint32_t
b7033_is_rt(uint32_t u32Policy)
{
	return (u32Policy == B7033_FIFO || u32Policy == B7033_RR) ? 1u : 0u;
}

static uint32_t
b7033_is_dl(uint32_t u32Policy)
{
	return (u32Policy == B7033_DEADLINE) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sched_attr_policy_idle_7033 - soft SCHED_IDLE.
 *
 * Always returns 5. Soft pure-data policy tag; no sched_setattr call.
 */
uint32_t
gj_sched_attr_policy_idle_7033(void)
{
	(void)NULL;
	return b7033_idle();
}

/*
 * gj_sched_attr_policy_deadline_7033 - soft SCHED_DEADLINE.
 *
 * Always returns 6.
 */
uint32_t
gj_sched_attr_policy_deadline_7033(void)
{
	return b7033_deadline();
}

/*
 * gj_sched_attr_policy_is_rt_7033 - soft real-time policy predicate.
 *
 * policy: soft sched_policy value
 *
 * Returns 1 when policy is FIFO or RR; else 0.
 */
uint32_t
gj_sched_attr_policy_is_rt_7033(uint32_t u32Policy)
{
	return b7033_is_rt(u32Policy);
}

/*
 * gj_sched_attr_policy_is_dl_7033 - soft deadline policy predicate.
 *
 * policy: soft sched_policy value
 *
 * Returns 1 when policy is DEADLINE; else 0.
 */
uint32_t
gj_sched_attr_policy_is_dl_7033(uint32_t u32Policy)
{
	return b7033_is_dl(u32Policy);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_sched_attr_policy_idle_7033(void)
    __attribute__((alias("gj_sched_attr_policy_idle_7033")));

uint32_t __gj_sched_attr_policy_deadline_7033(void)
    __attribute__((alias("gj_sched_attr_policy_deadline_7033")));

uint32_t __gj_sched_attr_policy_is_rt_7033(uint32_t u32Policy)
    __attribute__((alias("gj_sched_attr_policy_is_rt_7033")));

uint32_t __gj_sched_attr_policy_is_dl_7033(uint32_t u32Policy)
    __attribute__((alias("gj_sched_attr_policy_is_dl_7033")));
