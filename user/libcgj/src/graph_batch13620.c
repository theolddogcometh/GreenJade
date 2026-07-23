/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13620: proton soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13620(void);
 *     - Returns the compile-time graph batch number for this TU (13620).
 *   uint32_t __gj_batch_id_13620  (alias)
 *   __libcgj_batch13620_marker = "libcgj-batch13620"
 *
 * Exclusive continuum CREATE-ONLY (13611-13620: proton soft all→0 —
 * proton_ok_u_13611, wine_ok_u_13612, proton_ge_ok_u_13613,
 * steam_runtime_ok_u_13614, pressure_vessel_ok_u_13615,
 * compat_tool_ok_u_13616, dxvk_ok_u_13617, vkd3d_ok_u_13618,
 * proton_soft_ready_u_13619, batch_id_13620→13620). Unique surfaces
 * only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * Distinct from gj_batch_id_13420 (batch13420), gj_batch_id_13220
 * (batch13220), gj_batch_id_13020 (batch13020), and gj_batch_id_12820
 * (batch12820). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13620_marker[] = "libcgj-batch13620";

#define B13620_BATCH_ID  13620u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13620_id(void)
{
	return B13620_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13620 - report this TU's graph batch number.
 *
 * Always returns 13620. Closes the 13611-13620 proton soft wave.
 */
uint32_t
gj_batch_id_13620(void)
{
	(void)NULL;
	return b13620_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13620(void)
    __attribute__((alias("gj_batch_id_13620")));
