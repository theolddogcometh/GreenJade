/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13280: power/thermal soft product continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13280(void);
 *     - Returns the compile-time graph batch number for this TU (13280).
 *   uint32_t __gj_batch_id_13280  (alias)
 *   __libcgj_batch13280_marker = "libcgj-batch13280"
 *
 * Exclusive continuum CREATE-ONLY (13271-13280: power/thermal soft
 * product stubs — power_ok_u_13271, tdp_ok_u_13272,
 * battery_ok_u_13273, thermal_ok_u_13274, fan_ok_u_13275,
 * suspend_ok_u_13276, resume_ok_u_13277, performance_ok_u_13278,
 * power_soft_ready_u_13279, batch_id_13280). Unique surfaces only;
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

const char __libcgj_batch13280_marker[] = "libcgj-batch13280";

#define B13280_BATCH_ID  13280u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13280_id(void)
{
	return B13280_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13280 - report this TU's graph batch number.
 *
 * Always returns 13280.
 */
uint32_t
gj_batch_id_13280(void)
{
	(void)NULL;
	return b13280_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13280(void)
    __attribute__((alias("gj_batch_id_13280")));
