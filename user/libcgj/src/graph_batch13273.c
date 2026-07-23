/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13273: battery ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_battery_ok_u_13273(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       battery / fuel-gauge runtime probe for the power/thermal soft
 *       product continuum.
 *   uint32_t __gj_battery_ok_u_13273  (alias)
 *   __libcgj_batch13273_marker = "libcgj-batch13273"
 *
 * Exclusive continuum CREATE-ONLY (13271-13280: power/thermal soft
 * product stubs — power_ok_u_13271, tdp_ok_u_13272,
 * battery_ok_u_13273, thermal_ok_u_13274, fan_ok_u_13275,
 * suspend_ok_u_13276, resume_ok_u_13277, performance_ok_u_13278,
 * power_soft_ready_u_13279, batch_id_13280). Unique surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13273_marker[] = "libcgj-batch13273";

/* Soft battery-ok lamp: always off (not a real fuel-gauge probe). */
#define B13273_BATTERY_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13273_ok(void)
{
	return B13273_BATTERY_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_battery_ok_u_13273 - battery ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe battery state
 * or call libc. No parent wires.
 */
uint32_t
gj_battery_ok_u_13273(void)
{
	(void)NULL;
	return b13273_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_battery_ok_u_13273(void)
    __attribute__((alias("gj_battery_ok_u_13273")));
