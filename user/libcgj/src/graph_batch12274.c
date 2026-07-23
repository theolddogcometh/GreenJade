/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12274: thermal ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_thermal_ok_u_12274(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       thermal zone / temperature sensor runtime probe for the
 *       power/thermal soft product continuum.
 *   uint32_t __gj_thermal_ok_u_12274  (alias)
 *   __libcgj_batch12274_marker = "libcgj-batch12274"
 *
 * Exclusive continuum CREATE-ONLY (12271-12280: power/thermal soft
 * product stubs — power_ok_u_12271, tdp_ok_u_12272,
 * battery_ok_u_12273, thermal_ok_u_12274, fan_ok_u_12275,
 * suspend_ok_u_12276, resume_ok_u_12277, performance_ok_u_12278,
 * power_soft_ready_u_12279, batch_id_12280). Unique surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12274_marker[] = "libcgj-batch12274";

/* Soft thermal-ok lamp: always off (not a real thermal-zone probe). */
#define B12274_THERMAL_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12274_ok(void)
{
	return B12274_THERMAL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_thermal_ok_u_12274 - thermal ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe thermal zones
 * or call libc. No parent wires.
 */
uint32_t
gj_thermal_ok_u_12274(void)
{
	(void)NULL;
	return b12274_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_thermal_ok_u_12274(void)
    __attribute__((alias("gj_thermal_ok_u_12274")));
