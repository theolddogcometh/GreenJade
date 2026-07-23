/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12273: battery ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_battery_ok_u_12273(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       battery / fuel-gauge runtime probe for the power/thermal soft
 *       product continuum.
 *   uint32_t __gj_battery_ok_u_12273  (alias)
 *   __libcgj_batch12273_marker = "libcgj-batch12273"
 *
 * Exclusive continuum CREATE-ONLY (12271-12280: power/thermal soft
 * product stubs — power_ok_u_12271, tdp_ok_u_12272,
 * battery_ok_u_12273, thermal_ok_u_12274, fan_ok_u_12275,
 * suspend_ok_u_12276, resume_ok_u_12277, performance_ok_u_12278,
 * power_soft_ready_u_12279, batch_id_12280). Unique surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12273_marker[] = "libcgj-batch12273";

/* Soft battery-ok lamp: always off (not a real fuel-gauge probe). */
#define B12273_BATTERY_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12273_ok(void)
{
	return B12273_BATTERY_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_battery_ok_u_12273 - battery ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe battery state
 * or call libc. No parent wires.
 */
uint32_t
gj_battery_ok_u_12273(void)
{
	(void)NULL;
	return b12273_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_battery_ok_u_12273(void)
    __attribute__((alias("gj_battery_ok_u_12273")));
