/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13075: fan ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_fan_ok_u_13075(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       fan / cooling-device runtime probe for the power/thermal soft
 *       product continuum.
 *   uint32_t __gj_fan_ok_u_13075  (alias)
 *   __libcgj_batch13075_marker = "libcgj-batch13075"
 *
 * Exclusive continuum CREATE-ONLY (13071-13080: power/thermal soft
 * product stubs — power_ok_u_13071, tdp_ok_u_13072,
 * battery_ok_u_13073, thermal_ok_u_13074, fan_ok_u_13075,
 * suspend_ok_u_13076, resume_ok_u_13077, performance_ok_u_13078,
 * power_soft_ready_u_13079, batch_id_13080). Unique surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13075_marker[] = "libcgj-batch13075";

/* Soft fan-ok lamp: always off (not a real cooling-device probe). */
#define B13075_FAN_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13075_ok(void)
{
	return B13075_FAN_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fan_ok_u_13075 - fan ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe fan hardware
 * or call libc. No parent wires.
 */
uint32_t
gj_fan_ok_u_13075(void)
{
	(void)NULL;
	return b13075_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fan_ok_u_13075(void)
    __attribute__((alias("gj_fan_ok_u_13075")));
