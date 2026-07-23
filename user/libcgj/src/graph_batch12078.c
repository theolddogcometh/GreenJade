/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12078: performance ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_performance_ok_u_12078(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       performance / CPU-freq / power-profile probe for the power soft
 *       continuum.
 *   uint32_t __gj_performance_ok_u_12078  (alias)
 *   __libcgj_batch12078_marker = "libcgj-batch12078"
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

const char __libcgj_batch12078_marker[] = "libcgj-batch12078";

/* Soft performance-ok lamp: always off (not a real power-profile probe). */
#define B12078_PERFORMANCE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12078_performance_ok(void)
{
	return B12078_PERFORMANCE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_performance_ok_u_12078 - performance ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe performance
 * governors or call libc. No parent wires.
 */
uint32_t
gj_performance_ok_u_12078(void)
{
	(void)NULL;
	return b12078_performance_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_performance_ok_u_12078(void)
    __attribute__((alias("gj_performance_ok_u_12078")));
