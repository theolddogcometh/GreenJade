/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10280: power/thermal soft product continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10280(void);
 *     - Returns the compile-time graph batch number for this TU (10280).
 *   uint32_t __gj_batch_id_10280  (alias)
 *   __libcgj_batch10280_marker = "libcgj-batch10280"
 *
 * Exclusive continuum CREATE-ONLY (10271-10280: power/thermal soft
 * product stubs — power_ok_u_10271, tdp_ok_u_10272,
 * battery_ok_u_10273, thermal_ok_u_10274, fan_ok_u_10275,
 * suspend_ok_u_10276, resume_ok_u_10277, performance_ok_u_10278,
 * power_soft_ready_u_10279, batch_id_10280). Unique surfaces only;
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

const char __libcgj_batch10280_marker[] = "libcgj-batch10280";

#define B10280_BATCH_ID  10280u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10280_id(void)
{
	return B10280_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10280 - report this TU's graph batch number.
 *
 * Always returns 10280.
 */
uint32_t
gj_batch_id_10280(void)
{
	(void)NULL;
	return b10280_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10280(void)
    __attribute__((alias("gj_batch_id_10280")));
