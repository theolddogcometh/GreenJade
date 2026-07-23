/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10978: performance ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_performance_ok_u_10978(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       performance / CPU-freq / power-profile runtime probe for the
 *       power/thermal soft product continuum.
 *   uint32_t __gj_performance_ok_u_10978  (alias)
 *   __libcgj_batch10978_marker = "libcgj-batch10978"
 *
 * Exclusive continuum CREATE-ONLY (10971-10980: power/thermal soft
 * product stubs — power_ok_u_10971, tdp_ok_u_10972,
 * battery_ok_u_10973, thermal_ok_u_10974, fan_ok_u_10975,
 * suspend_ok_u_10976, resume_ok_u_10977, performance_ok_u_10978,
 * power_soft_ready_u_10979, batch_id_10980). Unique surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10978_marker[] = "libcgj-batch10978";

/* Soft performance-ok lamp: always off (not a real power-profile probe). */
#define B10978_PERFORMANCE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10978_ok(void)
{
	return B10978_PERFORMANCE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_performance_ok_u_10978 - performance ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe performance
 * governors or call libc. No parent wires.
 */
uint32_t
gj_performance_ok_u_10978(void)
{
	(void)NULL;
	return b10978_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_performance_ok_u_10978(void)
    __attribute__((alias("gj_performance_ok_u_10978")));
