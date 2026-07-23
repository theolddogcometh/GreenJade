/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12075: fan ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_fan_ok_u_12075(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       fan / cooling-device probe for the power soft continuum.
 *   uint32_t __gj_fan_ok_u_12075  (alias)
 *   __libcgj_batch12075_marker = "libcgj-batch12075"
 *
 * Exclusive continuum CREATE-ONLY (12071-12080: power soft all→0 —
 * power_ok_u_12071, tdp_ok_u_12072, battery_ok_u_12073,
 * thermal_ok_u_12074, fan_ok_u_12075, suspend_ok_u_12076,
 * resume_ok_u_12077, performance_ok_u_12078, power_soft_ready_u_12079,
 * batch_id_12080). Unique surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12075_marker[] = "libcgj-batch12075";

/* Soft fan-ok lamp: always off (not a real cooling-device probe). */
#define B12075_FAN_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12075_fan_ok(void)
{
	return B12075_FAN_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fan_ok_u_12075 - fan ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe fan hardware
 * or call libc. No parent wires.
 */
uint32_t
gj_fan_ok_u_12075(void)
{
	(void)NULL;
	return b12075_fan_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fan_ok_u_12075(void)
    __attribute__((alias("gj_fan_ok_u_12075")));
