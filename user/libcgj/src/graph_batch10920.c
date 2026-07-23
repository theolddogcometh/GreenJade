/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10920: proton soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10920(void);
 *     - Returns the compile-time graph batch number for this TU (10920).
 *   uint32_t __gj_batch_id_10920  (alias)
 *   __libcgj_batch10920_marker = "libcgj-batch10920"
 *
 * Exclusive continuum CREATE-ONLY (10911-10920: proton soft all→0 —
 * proton_ok_u_10911, wine_ok_u_10912, proton_ge_ok_u_10913,
 * steam_runtime_ok_u_10914, pressure_vessel_ok_u_10915,
 * compat_tool_ok_u_10916, dxvk_ok_u_10917, vkd3d_ok_u_10918,
 * proton_soft_ready_u_10919, batch_id_10920→10920). Unique surface
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

const char __libcgj_batch10920_marker[] = "libcgj-batch10920";

#define B10920_BATCH_ID  10920u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10920_id(void)
{
	return B10920_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10920 - report this TU's graph batch number.
 *
 * Always returns 10920. Useful for link-time presence checks and
 * runtime graph revision probes without string compares on the marker.
 * No parent wires.
 */
uint32_t
gj_batch_id_10920(void)
{
	(void)NULL;
	return b10920_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10920(void)
    __attribute__((alias("gj_batch_id_10920")));
