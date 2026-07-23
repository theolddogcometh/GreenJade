/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12476: suspend ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_suspend_ok_u_12476(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       system suspend / S3-S0ix capability probe for the power/thermal
 *       soft product continuum.
 *   uint32_t __gj_suspend_ok_u_12476  (alias)
 *   __libcgj_batch12476_marker = "libcgj-batch12476"
 *
 * Exclusive continuum CREATE-ONLY (12471-12480: power/thermal soft
 * product stubs — power_ok_u_12471, tdp_ok_u_12472,
 * battery_ok_u_12473, thermal_ok_u_12474, fan_ok_u_12475,
 * suspend_ok_u_12476, resume_ok_u_12477, performance_ok_u_12478,
 * power_soft_ready_u_12479, batch_id_12480). Unique surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12476_marker[] = "libcgj-batch12476";

/* Soft suspend-ok lamp: always off (not a real suspend capability probe). */
#define B12476_SUSPEND_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12476_ok(void)
{
	return B12476_SUSPEND_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_suspend_ok_u_12476 - suspend ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe suspend state
 * or call libc. No parent wires.
 */
uint32_t
gj_suspend_ok_u_12476(void)
{
	(void)NULL;
	return b12476_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_suspend_ok_u_12476(void)
    __attribute__((alias("gj_suspend_ok_u_12476")));
