/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13876: suspend ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_suspend_ok_u_13876(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       suspend/sleep state probe for the power soft continuum.
 *   uint32_t __gj_suspend_ok_u_13876  (alias)
 *   __libcgj_batch13876_marker = "libcgj-batch13876"
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

const char __libcgj_batch13876_marker[] = "libcgj-batch13876";

/* Soft suspend-ok lamp: always off (not a real suspend probe). */
#define B13876_SUSPEND_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13876_suspend_ok(void)
{
	return B13876_SUSPEND_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_suspend_ok_u_13876 - suspend ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe sleep states
 * or call libc. No parent wires.
 */
uint32_t
gj_suspend_ok_u_13876(void)
{
	(void)NULL;
	return b13876_suspend_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_suspend_ok_u_13876(void)
    __attribute__((alias("gj_suspend_ok_u_13876")));
