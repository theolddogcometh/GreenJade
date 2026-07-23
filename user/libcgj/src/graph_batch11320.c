/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11320: proton soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_11320(void);
 *     - Returns the compile-time graph batch number for this TU (11320).
 *   uint32_t __gj_batch_id_11320  (alias)
 *   __libcgj_batch11320_marker = "libcgj-batch11320"
 *
 * Exclusive continuum CREATE-ONLY (11311-11320: proton soft all→0 —
 * proton_ok_u_11311, wine_ok_u_11312, proton_ge_ok_u_11313,
 * steam_runtime_ok_u_11314, pressure_vessel_ok_u_11315,
 * compat_tool_ok_u_11316, dxvk_ok_u_11317, vkd3d_ok_u_11318,
 * proton_soft_ready_u_11319, batch_id_11320→11320). Unique surface
 * only; no multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone
 * / prior batch_id_* symbols — avoid multi-def. Distinct from
 * gj_batch_id_11120 (batch11120) and gj_batch_id_10920 (batch10920).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11320_marker[] = "libcgj-batch11320";

#define B11320_BATCH_ID  11320u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11320_id(void)
{
	return B11320_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_11320 - report this TU's graph batch number.
 *
 * Always returns 11320. Useful for link-time presence checks and
 * runtime graph revision probes without string compares on the marker.
 * No parent wires.
 */
uint32_t
gj_batch_id_11320(void)
{
	(void)NULL;
	return b11320_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_11320(void)
    __attribute__((alias("gj_batch_id_11320")));
