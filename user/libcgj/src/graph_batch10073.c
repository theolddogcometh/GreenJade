/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10073: battery ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_battery_ok_u_10073(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       battery/sysfs probe for the power/thermal continuum.
 *   uint32_t __gj_battery_ok_u_10073  (alias)
 *   __libcgj_batch10073_marker = "libcgj-batch10073"
 *
 * Exclusive continuum CREATE-ONLY (10071-10080: power/thermal soft id
 * stubs — tdp_ok_u_10071, fan_ok_u_10072, battery_ok_u_10073,
 * thermal_ok_u_10074, cpu_boost_ok_u_10075, gpu_clock_ok_u_10076,
 * suspend_ok_u_10077, resume_ok_u_10078, power_soft_ready_u_10079,
 * batch_id_10080). Unique surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10073_marker[] = "libcgj-batch10073";

/* Soft battery-ok lamp: always off (not a real battery probe). */
#define B10073_BATTERY_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10073_battery_ok(void)
{
	return B10073_BATTERY_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_battery_ok_u_10073 - battery ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe power_supply
 * or call libc. No parent wires.
 */
uint32_t
gj_battery_ok_u_10073(void)
{
	(void)NULL;
	return b10073_battery_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_battery_ok_u_10073(void)
    __attribute__((alias("gj_battery_ok_u_10073")));
