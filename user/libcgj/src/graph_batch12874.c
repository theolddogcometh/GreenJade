/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12874: thermal ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_thermal_ok_u_12874(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       thermal zone / temperature sensor runtime probe for the
 *       power/thermal soft product continuum.
 *   uint32_t __gj_thermal_ok_u_12874  (alias)
 *   __libcgj_batch12874_marker = "libcgj-batch12874"
 *
 * Exclusive continuum CREATE-ONLY (12871-12880: power/thermal soft
 * product stubs — power_ok_u_12871, tdp_ok_u_12872,
 * battery_ok_u_12873, thermal_ok_u_12874, fan_ok_u_12875,
 * suspend_ok_u_12876, resume_ok_u_12877, performance_ok_u_12878,
 * power_soft_ready_u_12879, batch_id_12880). Unique surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12874_marker[] = "libcgj-batch12874";

/* Soft thermal-ok lamp: always off (not a real thermal-zone probe). */
#define B12874_THERMAL_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12874_ok(void)
{
	return B12874_THERMAL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_thermal_ok_u_12874 - thermal ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe thermal zones
 * or call libc. No parent wires.
 */
uint32_t
gj_thermal_ok_u_12874(void)
{
	(void)NULL;
	return b12874_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_thermal_ok_u_12874(void)
    __attribute__((alias("gj_thermal_ok_u_12874")));
