/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10371: power ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_power_ok_u_10371(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       power/sysfs probe for the power soft continuum.
 *   uint32_t __gj_power_ok_u_10371  (alias)
 *   __libcgj_batch10371_marker = "libcgj-batch10371"
 *
 * Exclusive continuum CREATE-ONLY (10371-10380: power soft all→0 —
 * power_ok_u_10371, tdp_ok_u_10372, battery_ok_u_10373,
 * thermal_ok_u_10374, fan_ok_u_10375, suspend_ok_u_10376,
 * resume_ok_u_10377, performance_ok_u_10378, power_soft_ready_u_10379,
 * batch_id_10380). Unique surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10371_marker[] = "libcgj-batch10371";

/* Soft power-ok lamp: always off (not a real power probe). */
#define B10371_POWER_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10371_power_ok(void)
{
	return B10371_POWER_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_power_ok_u_10371 - power ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe power/sysfs or
 * call libc. No parent wires.
 */
uint32_t
gj_power_ok_u_10371(void)
{
	(void)NULL;
	return b10371_power_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_power_ok_u_10371(void)
    __attribute__((alias("gj_power_ok_u_10371")));
