/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10072: fan ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_fan_ok_u_10072(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       fan/tachometer probe for the power/thermal continuum.
 *   uint32_t __gj_fan_ok_u_10072  (alias)
 *   __libcgj_batch10072_marker = "libcgj-batch10072"
 *
 * Exclusive continuum CREATE-ONLY (10071-10080: power/thermal soft id
 * stubs — tdp_ok_u_10071, fan_ok_u_10072, battery_ok_u_10073,
 * thermal_ok_u_10074, cpu_boost_ok_u_10075, gpu_clock_ok_u_10076,
 * suspend_ok_u_10077, resume_ok_u_10078, power_soft_ready_u_10079,
 * batch_id_10080). Unique surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10072_marker[] = "libcgj-batch10072";

/* Soft fan-ok lamp: always off (not a real fan probe). */
#define B10072_FAN_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10072_fan_ok(void)
{
	return B10072_FAN_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fan_ok_u_10072 - fan ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe fan hwmon or
 * call libc. No parent wires.
 */
uint32_t
gj_fan_ok_u_10072(void)
{
	(void)NULL;
	return b10072_fan_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fan_ok_u_10072(void)
    __attribute__((alias("gj_fan_ok_u_10072")));
