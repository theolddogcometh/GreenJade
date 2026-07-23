/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13420: proton soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13420(void);
 *     - Returns the compile-time graph batch number for this TU (13420).
 *   uint32_t __gj_batch_id_13420  (alias)
 *   __libcgj_batch13420_marker = "libcgj-batch13420"
 *
 * Exclusive continuum CREATE-ONLY (13411-13420: proton soft all→0 —
 * proton_ok_u_13411, wine_ok_u_13412, proton_ge_ok_u_13413,
 * steam_runtime_ok_u_13414, pressure_vessel_ok_u_13415,
 * compat_tool_ok_u_13416, dxvk_ok_u_13417, vkd3d_ok_u_13418,
 * proton_soft_ready_u_13419, batch_id_13420→13420). Unique surfaces
 * only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * Distinct from gj_batch_id_13220 (batch13220), gj_batch_id_13020
 * (batch13020), and gj_batch_id_12820 (batch12820). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13420_marker[] = "libcgj-batch13420";

#define B13420_BATCH_ID  13420u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13420_id(void)
{
	return B13420_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13420 - report this TU's graph batch number.
 *
 * Always returns 13420.
 */
uint32_t
gj_batch_id_13420(void)
{
	(void)NULL;
	return b13420_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13420(void)
    __attribute__((alias("gj_batch_id_13420")));
