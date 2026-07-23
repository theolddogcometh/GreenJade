/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12672: tdp ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_tdp_ok_u_12672(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       TDP / thermal design power configuration probe for the
 *       power/thermal soft product continuum.
 *   uint32_t __gj_tdp_ok_u_12672  (alias)
 *   __libcgj_batch12672_marker = "libcgj-batch12672"
 *
 * Exclusive continuum CREATE-ONLY (12671-12680: power/thermal soft
 * product stubs — power_ok_u_12671, tdp_ok_u_12672,
 * battery_ok_u_12673, thermal_ok_u_12674, fan_ok_u_12675,
 * suspend_ok_u_12676, resume_ok_u_12677, performance_ok_u_12678,
 * power_soft_ready_u_12679, batch_id_12680). Unique surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12672_marker[] = "libcgj-batch12672";

/* Soft tdp-ok lamp: always off (not a real TDP configuration probe). */
#define B12672_TDP_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12672_ok(void)
{
	return B12672_TDP_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tdp_ok_u_12672 - tdp ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe TDP limits
 * or call libc. No parent wires.
 */
uint32_t
gj_tdp_ok_u_12672(void)
{
	(void)NULL;
	return b12672_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_tdp_ok_u_12672(void)
    __attribute__((alias("gj_tdp_ok_u_12672")));
