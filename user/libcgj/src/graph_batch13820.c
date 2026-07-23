/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13820: proton soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13820(void);
 *     - Returns the compile-time graph batch number for this TU (13820).
 *   uint32_t __gj_batch_id_13820  (alias)
 *   __libcgj_batch13820_marker = "libcgj-batch13820"
 *
 * Exclusive continuum CREATE-ONLY (13811-13820: proton soft all→0 —
 * proton_ok_u_13811, wine_ok_u_13812, proton_ge_ok_u_13813,
 * steam_runtime_ok_u_13814, pressure_vessel_ok_u_13815,
 * compat_tool_ok_u_13816, dxvk_ok_u_13817, vkd3d_ok_u_13818,
 * proton_soft_ready_u_13819, batch_id_13820→13820). Unique surfaces
 * only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * Distinct from gj_batch_id_13620 (batch13620), gj_batch_id_13420
 * (batch13420), gj_batch_id_13220 (batch13220), and gj_batch_id_13020
 * (batch13020). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13820_marker[] = "libcgj-batch13820";

#define B13820_BATCH_ID  13820u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13820_id(void)
{
	return B13820_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13820 - report this TU's graph batch number.
 *
 * Always returns 13820. Closes the 13811-13820 proton soft wave.
 */
uint32_t
gj_batch_id_13820(void)
{
	(void)NULL;
	return b13820_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13820(void)
    __attribute__((alias("gj_batch_id_13820")));
