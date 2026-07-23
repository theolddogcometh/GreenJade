/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10574: thermal ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_thermal_ok_u_10574(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       thermal zone probe for the power soft continuum.
 *   uint32_t __gj_thermal_ok_u_10574  (alias)
 *   __libcgj_batch10574_marker = "libcgj-batch10574"
 *
 * Exclusive continuum CREATE-ONLY (10571-10580: power soft all→0 —
 * power_ok_u_10571, tdp_ok_u_10572, battery_ok_u_10573,
 * thermal_ok_u_10574, fan_ok_u_10575, suspend_ok_u_10576,
 * resume_ok_u_10577, performance_ok_u_10578, power_soft_ready_u_10579,
 * batch_id_10580). Unique surface only; no multi-def. Distinct from
 * gj_thermal_ok_u_10074 (batch10074). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10574_marker[] = "libcgj-batch10574";

/* Soft thermal-ok lamp: always off (not a real thermal probe). */
#define B10574_THERMAL_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10574_ok(void)
{
	return B10574_THERMAL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_thermal_ok_u_10574 - thermal ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe thermal zones
 * or call libc. No parent wires.
 */
uint32_t
gj_thermal_ok_u_10574(void)
{
	(void)NULL;
	return b10574_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_thermal_ok_u_10574(void)
    __attribute__((alias("gj_thermal_ok_u_10574")));
