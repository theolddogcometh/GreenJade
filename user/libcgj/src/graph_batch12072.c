/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12072: tdp ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_tdp_ok_u_12072(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       TDP / thermal design power probe for the power soft continuum.
 *   uint32_t __gj_tdp_ok_u_12072  (alias)
 *   __libcgj_batch12072_marker = "libcgj-batch12072"
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

const char __libcgj_batch12072_marker[] = "libcgj-batch12072";

/* Soft tdp-ok lamp: always off (not a real TDP probe). */
#define B12072_TDP_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12072_tdp_ok(void)
{
	return B12072_TDP_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tdp_ok_u_12072 - tdp ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe TDP/RAPL or
 * call libc. No parent wires.
 */
uint32_t
gj_tdp_ok_u_12072(void)
{
	(void)NULL;
	return b12072_tdp_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_tdp_ok_u_12072(void)
    __attribute__((alias("gj_tdp_ok_u_12072")));
