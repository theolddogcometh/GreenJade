/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11172: tdp ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_tdp_ok_u_11172(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       TDP / thermal design power configuration probe for the
 *       power/thermal soft product continuum.
 *   uint32_t __gj_tdp_ok_u_11172  (alias)
 *   __libcgj_batch11172_marker = "libcgj-batch11172"
 *
 * Exclusive continuum CREATE-ONLY (11171-11180: power/thermal soft
 * product stubs — power_ok_u_11171, tdp_ok_u_11172,
 * battery_ok_u_11173, thermal_ok_u_11174, fan_ok_u_11175,
 * suspend_ok_u_11176, resume_ok_u_11177, performance_ok_u_11178,
 * power_soft_ready_u_11179, batch_id_11180). Unique surface only;
 * no multi-def. Distinct from gj_tdp_ok_u_10972 (batch10972) and prior
 * tdp soft waves. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11172_marker[] = "libcgj-batch11172";

/* Soft tdp-ok lamp: always off (not a real TDP configuration probe). */
#define B11172_TDP_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11172_ok(void)
{
	return B11172_TDP_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tdp_ok_u_11172 - tdp ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe TDP limits
 * or call libc. No parent wires.
 */
uint32_t
gj_tdp_ok_u_11172(void)
{
	(void)NULL;
	return b11172_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_tdp_ok_u_11172(void)
    __attribute__((alias("gj_tdp_ok_u_11172")));
