/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12480: power/thermal soft product continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_12480(void);
 *     - Returns the compile-time graph batch number for this TU (12480).
 *   uint32_t __gj_batch_id_12480  (alias)
 *   __libcgj_batch12480_marker = "libcgj-batch12480"
 *
 * Exclusive continuum CREATE-ONLY (12471-12480: power/thermal soft
 * product stubs — power_ok_u_12471, tdp_ok_u_12472,
 * battery_ok_u_12473, thermal_ok_u_12474, fan_ok_u_12475,
 * suspend_ok_u_12476, resume_ok_u_12477, performance_ok_u_12478,
 * power_soft_ready_u_12479, batch_id_12480). Unique surfaces only;
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

const char __libcgj_batch12480_marker[] = "libcgj-batch12480";

#define B12480_BATCH_ID  12480u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12480_id(void)
{
	return B12480_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_12480 - report this TU's graph batch number.
 *
 * Always returns 12480.
 */
uint32_t
gj_batch_id_12480(void)
{
	(void)NULL;
	return b12480_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_12480(void)
    __attribute__((alias("gj_batch_id_12480")));
