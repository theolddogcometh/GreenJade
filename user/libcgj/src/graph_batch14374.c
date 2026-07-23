/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14374: thermal ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_thermal_ok_u_14374(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       thermal zone probe for the power soft continuum.
 *   uint32_t __gj_thermal_ok_u_14374  (alias)
 *   __libcgj_batch14374_marker = "libcgj-batch14374"
 *
 * Exclusive continuum CREATE-ONLY (14371-14380: power soft all→0 —
 * power_ok_u_14371, tdp_ok_u_14372, battery_ok_u_14373,
 * thermal_ok_u_14374, fan_ok_u_14375, suspend_ok_u_14376,
 * resume_ok_u_14377, performance_ok_u_14378, power_soft_ready_u_14379,
 * batch_id_14380). Unique surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14374_marker[] = "libcgj-batch14374";

/* Soft thermal-ok lamp: always off (not a real thermal probe). */
#define B14374_THERMAL_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14374_thermal_ok(void)
{
	return B14374_THERMAL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_thermal_ok_u_14374 - thermal ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe thermal_zone
 * or call libc. No parent wires.
 */
uint32_t
gj_thermal_ok_u_14374(void)
{
	(void)NULL;
	return b14374_thermal_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_thermal_ok_u_14374(void)
    __attribute__((alias("gj_thermal_ok_u_14374")));
