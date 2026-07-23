/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10575: fan ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_fan_ok_u_10575(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       fan/tachometer probe for the power soft continuum.
 *   uint32_t __gj_fan_ok_u_10575  (alias)
 *   __libcgj_batch10575_marker = "libcgj-batch10575"
 *
 * Exclusive continuum CREATE-ONLY (10571-10580: power soft all→0 —
 * power_ok_u_10571, tdp_ok_u_10572, battery_ok_u_10573,
 * thermal_ok_u_10574, fan_ok_u_10575, suspend_ok_u_10576,
 * resume_ok_u_10577, performance_ok_u_10578, power_soft_ready_u_10579,
 * batch_id_10580). Unique surface only; no multi-def. Distinct from
 * gj_fan_ok_u_10072 (batch10072). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10575_marker[] = "libcgj-batch10575";

/* Soft fan-ok lamp: always off (not a real fan probe). */
#define B10575_FAN_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10575_ok(void)
{
	return B10575_FAN_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fan_ok_u_10575 - fan ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe fan/tach or
 * call libc. No parent wires.
 */
uint32_t
gj_fan_ok_u_10575(void)
{
	(void)NULL;
	return b10575_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fan_ok_u_10575(void)
    __attribute__((alias("gj_fan_ok_u_10575")));
