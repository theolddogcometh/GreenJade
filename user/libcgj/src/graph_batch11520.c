/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11520: proton soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_11520(void);
 *     - Returns the compile-time graph batch number for this TU (11520).
 *   uint32_t __gj_batch_id_11520  (alias)
 *   __libcgj_batch11520_marker = "libcgj-batch11520"
 *
 * Exclusive continuum CREATE-ONLY (11511-11520: proton soft all→0 —
 * proton_ok_u_11511, wine_ok_u_11512, proton_ge_ok_u_11513,
 * steam_runtime_ok_u_11514, pressure_vessel_ok_u_11515,
 * compat_tool_ok_u_11516, dxvk_ok_u_11517, vkd3d_ok_u_11518,
 * proton_soft_ready_u_11519, batch_id_11520→11520). Unique surface
 * only; no multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone
 * / prior batch_id_* symbols — avoid multi-def. Distinct from
 * gj_batch_id_11320 (batch11320) and gj_batch_id_11120 (batch11120).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11520_marker[] = "libcgj-batch11520";

#define B11520_BATCH_ID  11520u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11520_id(void)
{
	return B11520_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_11520 - report this TU's graph batch number.
 *
 * Always returns 11520. Useful for link-time presence checks and
 * runtime graph revision probes without string compares on the marker.
 * No parent wires.
 */
uint32_t
gj_batch_id_11520(void)
{
	(void)NULL;
	return b11520_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_11520(void)
    __attribute__((alias("gj_batch_id_11520")));
