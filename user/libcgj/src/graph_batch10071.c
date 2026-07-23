/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10071: TDP ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_tdp_ok_u_10071(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       thermal design power probe for the power/thermal continuum.
 *   uint32_t __gj_tdp_ok_u_10071  (alias)
 *   __libcgj_batch10071_marker = "libcgj-batch10071"
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

const char __libcgj_batch10071_marker[] = "libcgj-batch10071";

/* Soft TDP-ok lamp: always off (not a real power probe). */
#define B10071_TDP_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10071_tdp_ok(void)
{
	return B10071_TDP_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tdp_ok_u_10071 - TDP ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe RAPL/TDP or
 * call libc. No parent wires.
 */
uint32_t
gj_tdp_ok_u_10071(void)
{
	(void)NULL;
	return b10071_tdp_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_tdp_ok_u_10071(void)
    __attribute__((alias("gj_tdp_ok_u_10071")));
