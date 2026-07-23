/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11173: battery ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_battery_ok_u_11173(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       battery / fuel-gauge runtime probe for the power/thermal soft
 *       product continuum.
 *   uint32_t __gj_battery_ok_u_11173  (alias)
 *   __libcgj_batch11173_marker = "libcgj-batch11173"
 *
 * Exclusive continuum CREATE-ONLY (11171-11180: power/thermal soft
 * product stubs — power_ok_u_11171, tdp_ok_u_11172,
 * battery_ok_u_11173, thermal_ok_u_11174, fan_ok_u_11175,
 * suspend_ok_u_11176, resume_ok_u_11177, performance_ok_u_11178,
 * power_soft_ready_u_11179, batch_id_11180). Unique surface only;
 * no multi-def. Distinct from gj_battery_ok_u_10973 (batch10973) and
 * prior battery soft waves. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11173_marker[] = "libcgj-batch11173";

/* Soft battery-ok lamp: always off (not a real fuel-gauge probe). */
#define B11173_BATTERY_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11173_ok(void)
{
	return B11173_BATTERY_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_battery_ok_u_11173 - battery ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe battery state
 * or call libc. No parent wires.
 */
uint32_t
gj_battery_ok_u_11173(void)
{
	(void)NULL;
	return b11173_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_battery_ok_u_11173(void)
    __attribute__((alias("gj_battery_ok_u_11173")));
