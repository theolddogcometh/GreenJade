/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10374: thermal ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_thermal_ok_u_10374(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       thermal zone probe for the power soft continuum.
 *   uint32_t __gj_thermal_ok_u_10374  (alias)
 *   __libcgj_batch10374_marker = "libcgj-batch10374"
 *
 * Exclusive continuum CREATE-ONLY (10371-10380: power soft all→0 —
 * power_ok_u_10371, tdp_ok_u_10372, battery_ok_u_10373,
 * thermal_ok_u_10374, fan_ok_u_10375, suspend_ok_u_10376,
 * resume_ok_u_10377, performance_ok_u_10378, power_soft_ready_u_10379,
 * batch_id_10380). Unique surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10374_marker[] = "libcgj-batch10374";

/* Soft thermal-ok lamp: always off (not a real thermal probe). */
#define B10374_THERMAL_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10374_thermal_ok(void)
{
	return B10374_THERMAL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_thermal_ok_u_10374 - thermal ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe thermal_zone
 * or call libc. No parent wires.
 */
uint32_t
gj_thermal_ok_u_10374(void)
{
	(void)NULL;
	return b10374_thermal_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_thermal_ok_u_10374(void)
    __attribute__((alias("gj_thermal_ok_u_10374")));
