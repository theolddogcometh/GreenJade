/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10277: resume ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_resume_ok_u_10277(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       system resume / wake-path capability probe for the power/thermal
 *       soft product continuum.
 *   uint32_t __gj_resume_ok_u_10277  (alias)
 *   __libcgj_batch10277_marker = "libcgj-batch10277"
 *
 * Exclusive continuum CREATE-ONLY (10271-10280: power/thermal soft
 * product stubs — power_ok_u_10271, tdp_ok_u_10272,
 * battery_ok_u_10273, thermal_ok_u_10274, fan_ok_u_10275,
 * suspend_ok_u_10276, resume_ok_u_10277, performance_ok_u_10278,
 * power_soft_ready_u_10279, batch_id_10280). Unique surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10277_marker[] = "libcgj-batch10277";

/* Soft resume-ok lamp: always off (not a real wake-path probe). */
#define B10277_RESUME_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10277_ok(void)
{
	return B10277_RESUME_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_resume_ok_u_10277 - resume ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe resume path
 * or call libc. No parent wires.
 */
uint32_t
gj_resume_ok_u_10277(void)
{
	(void)NULL;
	return b10277_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_resume_ok_u_10277(void)
    __attribute__((alias("gj_resume_ok_u_10277")));
