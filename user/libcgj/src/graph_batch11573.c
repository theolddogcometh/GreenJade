/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11573: battery ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_battery_ok_u_11573(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       battery/sysfs probe for the power soft continuum.
 *   uint32_t __gj_battery_ok_u_11573  (alias)
 *   __libcgj_batch11573_marker = "libcgj-batch11573"
 *
 * Exclusive continuum CREATE-ONLY (11571-11580: power soft all→0 —
 * power_ok_u_11571, tdp_ok_u_11572, battery_ok_u_11573,
 * thermal_ok_u_11574, fan_ok_u_11575, suspend_ok_u_11576,
 * resume_ok_u_11577, performance_ok_u_11578, power_soft_ready_u_11579,
 * batch_id_11580). Unique surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11573_marker[] = "libcgj-batch11573";

/* Soft battery-ok lamp: always off (not a real battery probe). */
#define B11573_BATTERY_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11573_battery_ok(void)
{
	return B11573_BATTERY_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_battery_ok_u_11573 - battery ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe power_supply
 * or call libc. No parent wires.
 */
uint32_t
gj_battery_ok_u_11573(void)
{
	(void)NULL;
	return b11573_battery_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_battery_ok_u_11573(void)
    __attribute__((alias("gj_battery_ok_u_11573")));
