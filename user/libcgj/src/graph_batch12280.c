/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12280: power/thermal soft product continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_12280(void);
 *     - Returns the compile-time graph batch number for this TU (12280).
 *   uint32_t __gj_batch_id_12280  (alias)
 *   __libcgj_batch12280_marker = "libcgj-batch12280"
 *
 * Exclusive continuum CREATE-ONLY (12271-12280: power/thermal soft
 * product stubs — power_ok_u_12271, tdp_ok_u_12272,
 * battery_ok_u_12273, thermal_ok_u_12274, fan_ok_u_12275,
 * suspend_ok_u_12276, resume_ok_u_12277, performance_ok_u_12278,
 * power_soft_ready_u_12279, batch_id_12280). Unique surfaces only;
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

const char __libcgj_batch12280_marker[] = "libcgj-batch12280";

#define B12280_BATCH_ID  12280u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12280_id(void)
{
	return B12280_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_12280 - report this TU's graph batch number.
 *
 * Always returns 12280.
 */
uint32_t
gj_batch_id_12280(void)
{
	(void)NULL;
	return b12280_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_12280(void)
    __attribute__((alias("gj_batch_id_12280")));
