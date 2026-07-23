/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11574: thermal ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_thermal_ok_u_11574(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       thermal zone probe for the power soft continuum.
 *   uint32_t __gj_thermal_ok_u_11574  (alias)
 *   __libcgj_batch11574_marker = "libcgj-batch11574"
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

const char __libcgj_batch11574_marker[] = "libcgj-batch11574";

/* Soft thermal-ok lamp: always off (not a real thermal probe). */
#define B11574_THERMAL_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11574_thermal_ok(void)
{
	return B11574_THERMAL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_thermal_ok_u_11574 - thermal ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe thermal_zone
 * or call libc. No parent wires.
 */
uint32_t
gj_thermal_ok_u_11574(void)
{
	(void)NULL;
	return b11574_thermal_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_thermal_ok_u_11574(void)
    __attribute__((alias("gj_thermal_ok_u_11574")));
