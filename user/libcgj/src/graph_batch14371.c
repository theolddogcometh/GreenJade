/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14371: power ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_power_ok_u_14371(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       power/sysfs probe for the power soft continuum.
 *   uint32_t __gj_power_ok_u_14371  (alias)
 *   __libcgj_batch14371_marker = "libcgj-batch14371"
 *
 * Exclusive continuum CREATE-ONLY (14371-14380: power soft all→0 —
 * power_ok_u_14371, tdp_ok_u_14372, battery_ok_u_14373,
 * thermal_ok_u_14374, fan_ok_u_14375, suspend_ok_u_14376,
 * resume_ok_u_14377, performance_ok_u_14378, power_soft_ready_u_14379,
 * batch_id_14380). Unique surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14371_marker[] = "libcgj-batch14371";

/* Soft power-ok lamp: always off (not a real power probe). */
#define B14371_POWER_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14371_power_ok(void)
{
	return B14371_POWER_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_power_ok_u_14371 - power ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe power/sysfs or
 * call libc. No parent wires.
 */
uint32_t
gj_power_ok_u_14371(void)
{
	(void)NULL;
	return b14371_power_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_power_ok_u_14371(void)
    __attribute__((alias("gj_power_ok_u_14371")));
