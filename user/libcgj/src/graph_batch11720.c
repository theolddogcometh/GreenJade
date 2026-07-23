/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11720: proton soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_11720(void);
 *     - Returns the compile-time graph batch number for this TU (11720).
 *   uint32_t __gj_batch_id_11720  (alias)
 *   __libcgj_batch11720_marker = "libcgj-batch11720"
 *
 * Exclusive continuum CREATE-ONLY (11711-11720: proton soft all→0 —
 * proton_ok_u_11711, wine_ok_u_11712, proton_ge_ok_u_11713,
 * steam_runtime_ok_u_11714, pressure_vessel_ok_u_11715,
 * compat_tool_ok_u_11716, dxvk_ok_u_11717, vkd3d_ok_u_11718,
 * proton_soft_ready_u_11719, batch_id_11720→11720). Unique surface
 * only; no multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone
 * / prior batch_id_* symbols — avoid multi-def. Distinct from
 * gj_batch_id_11520 (batch11520), gj_batch_id_11320 (batch11320), and
 * gj_batch_id_11120 (batch11120). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11720_marker[] = "libcgj-batch11720";

#define B11720_BATCH_ID  11720u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11720_id(void)
{
	return B11720_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_11720 - report this TU's graph batch number.
 *
 * Always returns 11720. Useful for link-time presence checks and
 * runtime graph revision probes without string compares on the marker.
 * No parent wires.
 */
uint32_t
gj_batch_id_11720(void)
{
	(void)NULL;
	return b11720_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_11720(void)
    __attribute__((alias("gj_batch_id_11720")));
