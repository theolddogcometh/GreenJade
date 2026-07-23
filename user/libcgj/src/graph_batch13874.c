/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13874: thermal ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_thermal_ok_u_13874(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       thermal zone probe for the power soft continuum.
 *   uint32_t __gj_thermal_ok_u_13874  (alias)
 *   __libcgj_batch13874_marker = "libcgj-batch13874"
 *
 * Exclusive continuum CREATE-ONLY (13871-13880: power soft all→0 —
 * power_ok_u_13871, tdp_ok_u_13872, battery_ok_u_13873,
 * thermal_ok_u_13874, fan_ok_u_13875, suspend_ok_u_13876,
 * resume_ok_u_13877, performance_ok_u_13878, power_soft_ready_u_13879,
 * batch_id_13880). Unique surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13874_marker[] = "libcgj-batch13874";

/* Soft thermal-ok lamp: always off (not a real thermal probe). */
#define B13874_THERMAL_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13874_thermal_ok(void)
{
	return B13874_THERMAL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_thermal_ok_u_13874 - thermal ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe thermal_zone
 * or call libc. No parent wires.
 */
uint32_t
gj_thermal_ok_u_13874(void)
{
	(void)NULL;
	return b13874_thermal_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_thermal_ok_u_13874(void)
    __attribute__((alias("gj_thermal_ok_u_13874")));
