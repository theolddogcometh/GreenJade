/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10720: proton soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10720(void);
 *     - Returns the compile-time graph batch number for this TU (10720).
 *   uint32_t __gj_batch_id_10720  (alias)
 *   __libcgj_batch10720_marker = "libcgj-batch10720"
 *
 * Exclusive continuum CREATE-ONLY (10711-10720: proton soft all→0 —
 * proton_ok_u_10711, wine_ok_u_10712, proton_ge_ok_u_10713,
 * steam_runtime_ok_u_10714, pressure_vessel_ok_u_10715,
 * compat_tool_ok_u_10716, dxvk_ok_u_10717, vkd3d_ok_u_10718,
 * proton_soft_ready_u_10719, batch_id_10720→10720). Unique surface
 * only; no multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone
 * / prior batch_id_* symbols — avoid multi-def. No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10720_marker[] = "libcgj-batch10720";

#define B10720_BATCH_ID  10720u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10720_id(void)
{
	return B10720_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10720 - report this TU's graph batch number.
 *
 * Always returns 10720. Useful for link-time presence checks and
 * runtime graph revision probes without string compares on the marker.
 * No parent wires.
 */
uint32_t
gj_batch_id_10720(void)
{
	(void)NULL;
	return b10720_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10720(void)
    __attribute__((alias("gj_batch_id_10720")));
