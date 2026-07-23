/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13473: battery ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_battery_ok_u_13473(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       battery / fuel-gauge runtime probe for the power/thermal soft
 *       product continuum.
 *   uint32_t __gj_battery_ok_u_13473  (alias)
 *   __libcgj_batch13473_marker = "libcgj-batch13473"
 *
 * Exclusive continuum CREATE-ONLY (13471-13480: power/thermal soft
 * product stubs — power_ok_u_13471, tdp_ok_u_13472,
 * battery_ok_u_13473, thermal_ok_u_13474, fan_ok_u_13475,
 * suspend_ok_u_13476, resume_ok_u_13477, performance_ok_u_13478,
 * power_soft_ready_u_13479, batch_id_13480). Unique surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13473_marker[] = "libcgj-batch13473";

/* Soft battery-ok lamp: always off (not a real fuel-gauge probe). */
#define B13473_BATTERY_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13473_ok(void)
{
	return B13473_BATTERY_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_battery_ok_u_13473 - battery ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe battery state
 * or call libc. No parent wires.
 */
uint32_t
gj_battery_ok_u_13473(void)
{
	(void)NULL;
	return b13473_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_battery_ok_u_13473(void)
    __attribute__((alias("gj_battery_ok_u_13473")));
