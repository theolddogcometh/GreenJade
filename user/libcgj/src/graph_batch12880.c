/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12880: power/thermal soft product continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_12880(void);
 *     - Returns the compile-time graph batch number for this TU (12880).
 *   uint32_t __gj_batch_id_12880  (alias)
 *   __libcgj_batch12880_marker = "libcgj-batch12880"
 *
 * Exclusive continuum CREATE-ONLY (12871-12880: power/thermal soft
 * product stubs — power_ok_u_12871, tdp_ok_u_12872,
 * battery_ok_u_12873, thermal_ok_u_12874, fan_ok_u_12875,
 * suspend_ok_u_12876, resume_ok_u_12877, performance_ok_u_12878,
 * power_soft_ready_u_12879, batch_id_12880). Unique surfaces only;
 * no multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12880_marker[] = "libcgj-batch12880";

#define B12880_BATCH_ID  12880u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12880_id(void)
{
	return B12880_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_12880 - report this TU's graph batch number.
 *
 * Always returns 12880.
 */
uint32_t
gj_batch_id_12880(void)
{
	(void)NULL;
	return b12880_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_12880(void)
    __attribute__((alias("gj_batch_id_12880")));
