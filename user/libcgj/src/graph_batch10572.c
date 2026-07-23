/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10572: TDP ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_tdp_ok_u_10572(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       thermal design power probe for the power soft continuum.
 *   uint32_t __gj_tdp_ok_u_10572  (alias)
 *   __libcgj_batch10572_marker = "libcgj-batch10572"
 *
 * Exclusive continuum CREATE-ONLY (10571-10580: power soft all→0 —
 * power_ok_u_10571, tdp_ok_u_10572, battery_ok_u_10573,
 * thermal_ok_u_10574, fan_ok_u_10575, suspend_ok_u_10576,
 * resume_ok_u_10577, performance_ok_u_10578, power_soft_ready_u_10579,
 * batch_id_10580). Unique surface only; no multi-def. Distinct from
 * gj_tdp_ok_u_10071 (batch10071). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10572_marker[] = "libcgj-batch10572";

/* Soft TDP-ok lamp: always off (not a real power probe). */
#define B10572_TDP_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10572_ok(void)
{
	return B10572_TDP_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tdp_ok_u_10572 - TDP ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe RAPL/TDP or
 * call libc. No parent wires.
 */
uint32_t
gj_tdp_ok_u_10572(void)
{
	(void)NULL;
	return b10572_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_tdp_ok_u_10572(void)
    __attribute__((alias("gj_tdp_ok_u_10572")));
