/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10774: thermal ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_thermal_ok_u_10774(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       thermal zone probe for the power soft continuum.
 *   uint32_t __gj_thermal_ok_u_10774  (alias)
 *   __libcgj_batch10774_marker = "libcgj-batch10774"
 *
 * Exclusive continuum CREATE-ONLY (10771-10780: power soft all→0 —
 * power_ok_u_10771, tdp_ok_u_10772, battery_ok_u_10773,
 * thermal_ok_u_10774, fan_ok_u_10775, suspend_ok_u_10776,
 * resume_ok_u_10777, performance_ok_u_10778, power_soft_ready_u_10779,
 * batch_id_10780). Unique surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10774_marker[] = "libcgj-batch10774";

/* Soft thermal-ok lamp: always off (not a real thermal probe). */
#define B10774_THERMAL_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10774_thermal_ok(void)
{
	return B10774_THERMAL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_thermal_ok_u_10774 - thermal ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe thermal_zone
 * or call libc. No parent wires.
 */
uint32_t
gj_thermal_ok_u_10774(void)
{
	(void)NULL;
	return b10774_thermal_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_thermal_ok_u_10774(void)
    __attribute__((alias("gj_thermal_ok_u_10774")));
