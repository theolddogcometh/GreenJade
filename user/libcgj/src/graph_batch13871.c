/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13871: power ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_power_ok_u_13871(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       power/sysfs probe for the power soft continuum.
 *   uint32_t __gj_power_ok_u_13871  (alias)
 *   __libcgj_batch13871_marker = "libcgj-batch13871"
 *
 * Exclusive continuum CREATE-ONLY (13871-13880: power soft all→0 —
 * power_ok_u_13871, tdp_ok_u_13872, battery_ok_u_13873,
 * thermal_ok_u_13874, fan_ok_u_13875, suspend_ok_u_13876,
 * resume_ok_u_13877, performance_ok_u_13878, power_soft_ready_u_13879,
 * batch_id_13880). Unique surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13871_marker[] = "libcgj-batch13871";

/* Soft power-ok lamp: always off (not a real power probe). */
#define B13871_POWER_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13871_power_ok(void)
{
	return B13871_POWER_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_power_ok_u_13871 - power ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe power/sysfs or
 * call libc. No parent wires.
 */
uint32_t
gj_power_ok_u_13871(void)
{
	(void)NULL;
	return b13871_power_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_power_ok_u_13871(void)
    __attribute__((alias("gj_power_ok_u_13871")));
