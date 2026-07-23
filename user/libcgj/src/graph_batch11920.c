/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11920: proton soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_11920(void);
 *     - Returns the compile-time graph batch number for this TU (11920).
 *   uint32_t __gj_batch_id_11920  (alias)
 *   __libcgj_batch11920_marker = "libcgj-batch11920"
 *
 * Exclusive continuum CREATE-ONLY (11911-11920: proton soft all→0 —
 * proton_ok_u_11911, wine_ok_u_11912, proton_ge_ok_u_11913,
 * steam_runtime_ok_u_11914, pressure_vessel_ok_u_11915,
 * compat_tool_ok_u_11916, dxvk_ok_u_11917, vkd3d_ok_u_11918,
 * proton_soft_ready_u_11919, batch_id_11920→11920). Unique surface
 * only; no multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone
 * / prior batch_id_* symbols — avoid multi-def. Distinct from
 * gj_batch_id_11720 (batch11720), gj_batch_id_11520 (batch11520), and
 * gj_batch_id_11320 (batch11320). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11920_marker[] = "libcgj-batch11920";

#define B11920_BATCH_ID  11920u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11920_id(void)
{
	return B11920_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_11920 - report this TU's graph batch number.
 *
 * Always returns 11920. Useful for link-time presence checks and
 * runtime graph revision probes without string compares on the marker.
 * No parent wires.
 */
uint32_t
gj_batch_id_11920(void)
{
	(void)NULL;
	return b11920_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_11920(void)
    __attribute__((alias("gj_batch_id_11920")));
