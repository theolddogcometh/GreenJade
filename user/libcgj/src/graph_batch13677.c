/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13677: resume ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_resume_ok_u_13677(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       resume/wake probe for the power soft continuum.
 *   uint32_t __gj_resume_ok_u_13677  (alias)
 *   __libcgj_batch13677_marker = "libcgj-batch13677"
 *
 * Exclusive continuum CREATE-ONLY (13671-13680: power soft all→0 —
 * power_ok_u_13671, tdp_ok_u_13672, battery_ok_u_13673,
 * thermal_ok_u_13674, fan_ok_u_13675, suspend_ok_u_13676,
 * resume_ok_u_13677, performance_ok_u_13678, power_soft_ready_u_13679,
 * batch_id_13680). Unique surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13677_marker[] = "libcgj-batch13677";

/* Soft resume-ok lamp: always off (not a real resume probe). */
#define B13677_RESUME_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13677_resume_ok(void)
{
	return B13677_RESUME_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_resume_ok_u_13677 - resume ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe wake sources
 * or call libc. No parent wires.
 */
uint32_t
gj_resume_ok_u_13677(void)
{
	(void)NULL;
	return b13677_resume_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_resume_ok_u_13677(void)
    __attribute__((alias("gj_resume_ok_u_13677")));
