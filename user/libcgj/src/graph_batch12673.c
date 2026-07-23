/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12673: battery ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_battery_ok_u_12673(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       battery / fuel-gauge runtime probe for the power/thermal soft
 *       product continuum.
 *   uint32_t __gj_battery_ok_u_12673  (alias)
 *   __libcgj_batch12673_marker = "libcgj-batch12673"
 *
 * Exclusive continuum CREATE-ONLY (12671-12680: power/thermal soft
 * product stubs — power_ok_u_12671, tdp_ok_u_12672,
 * battery_ok_u_12673, thermal_ok_u_12674, fan_ok_u_12675,
 * suspend_ok_u_12676, resume_ok_u_12677, performance_ok_u_12678,
 * power_soft_ready_u_12679, batch_id_12680). Unique surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12673_marker[] = "libcgj-batch12673";

/* Soft battery-ok lamp: always off (not a real fuel-gauge probe). */
#define B12673_BATTERY_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12673_ok(void)
{
	return B12673_BATTERY_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_battery_ok_u_12673 - battery ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe battery state
 * or call libc. No parent wires.
 */
uint32_t
gj_battery_ok_u_12673(void)
{
	(void)NULL;
	return b12673_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_battery_ok_u_12673(void)
    __attribute__((alias("gj_battery_ok_u_12673")));
