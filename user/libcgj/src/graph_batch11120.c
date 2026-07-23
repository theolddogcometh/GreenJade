/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11120: proton soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_11120(void);
 *     - Returns the compile-time graph batch number for this TU (11120).
 *   uint32_t __gj_batch_id_11120  (alias)
 *   __libcgj_batch11120_marker = "libcgj-batch11120"
 *
 * Exclusive continuum CREATE-ONLY (11111-11120: proton soft all→0 —
 * proton_ok_u_11111, wine_ok_u_11112, proton_ge_ok_u_11113,
 * steam_runtime_ok_u_11114, pressure_vessel_ok_u_11115,
 * compat_tool_ok_u_11116, dxvk_ok_u_11117, vkd3d_ok_u_11118,
 * proton_soft_ready_u_11119, batch_id_11120→11120). Unique surface
 * only; no multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone
 * / prior batch_id_* symbols — avoid multi-def. Distinct from
 * gj_batch_id_10920 (batch10920) and gj_batch_id_10720 (batch10720).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11120_marker[] = "libcgj-batch11120";

#define B11120_BATCH_ID  11120u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11120_id(void)
{
	return B11120_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_11120 - report this TU's graph batch number.
 *
 * Always returns 11120. Useful for link-time presence checks and
 * runtime graph revision probes without string compares on the marker.
 * No parent wires.
 */
uint32_t
gj_batch_id_11120(void)
{
	(void)NULL;
	return b11120_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_11120(void)
    __attribute__((alias("gj_batch_id_11120")));
