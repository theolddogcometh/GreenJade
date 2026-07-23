/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12620: proton soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_12620(void);
 *     - Returns the compile-time graph batch number for this TU (12620).
 *   uint32_t __gj_batch_id_12620  (alias)
 *   __libcgj_batch12620_marker = "libcgj-batch12620"
 *
 * Exclusive continuum CREATE-ONLY (12611-12620: proton soft all→0 —
 * proton_ok_u_12611, wine_ok_u_12612, proton_ge_ok_u_12613,
 * steam_runtime_ok_u_12614, pressure_vessel_ok_u_12615,
 * compat_tool_ok_u_12616, dxvk_ok_u_12617, vkd3d_ok_u_12618,
 * proton_soft_ready_u_12619, batch_id_12620→12620). Unique surfaces
 * only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * Distinct from gj_batch_id_12220 (batch12220), gj_batch_id_12020
 * (batch12020), and gj_batch_id_10220 (batch10220). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12620_marker[] = "libcgj-batch12620";

#define B12620_BATCH_ID  12620u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12620_id(void)
{
	return B12620_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_12620 - report this TU's graph batch number.
 *
 * Always returns 12620.
 */
uint32_t
gj_batch_id_12620(void)
{
	(void)NULL;
	return b12620_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_12620(void)
    __attribute__((alias("gj_batch_id_12620")));
