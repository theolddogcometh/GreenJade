/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12275: fan ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_fan_ok_u_12275(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       fan / cooling-device runtime probe for the power/thermal soft
 *       product continuum.
 *   uint32_t __gj_fan_ok_u_12275  (alias)
 *   __libcgj_batch12275_marker = "libcgj-batch12275"
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

const char __libcgj_batch12275_marker[] = "libcgj-batch12275";

/* Soft fan-ok lamp: always off (not a real cooling-device probe). */
#define B12275_FAN_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12275_ok(void)
{
	return B12275_FAN_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fan_ok_u_12275 - fan ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe fan hardware
 * or call libc. No parent wires.
 */
uint32_t
gj_fan_ok_u_12275(void)
{
	(void)NULL;
	return b12275_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fan_ok_u_12275(void)
    __attribute__((alias("gj_fan_ok_u_12275")));
