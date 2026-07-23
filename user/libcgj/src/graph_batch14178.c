/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14178: resume ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_resume_ok_u_14178(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       resume/wake probe for the power/thermal continuum.
 *   uint32_t __gj_resume_ok_u_14178  (alias)
 *   __libcgj_batch14178_marker = "libcgj-batch14178"
 *
 * Exclusive continuum CREATE-ONLY (14171-14180: power/thermal soft id
 * stubs — tdp_ok_u_14171, fan_ok_u_14172, battery_ok_u_14173,
 * thermal_ok_u_14174, cpu_boost_ok_u_14175, gpu_clock_ok_u_14176,
 * suspend_ok_u_14177, resume_ok_u_14178, power_soft_ready_u_14179,
 * batch_id_14180). Unique surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14178_marker[] = "libcgj-batch14178";

/* Soft resume-ok lamp: always off (not a real resume probe). */
#define B14178_RESUME_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14178_resume_ok(void)
{
	return B14178_RESUME_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_resume_ok_u_14178 - resume ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe wake sources
 * or call libc. No parent wires.
 */
uint32_t
gj_resume_ok_u_14178(void)
{
	(void)NULL;
	return b14178_resume_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_resume_ok_u_14178(void)
    __attribute__((alias("gj_resume_ok_u_14178")));
