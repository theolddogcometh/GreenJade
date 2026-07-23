/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12820: proton soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_12820(void);
 *     - Returns the compile-time graph batch number for this TU (12820).
 *   uint32_t __gj_batch_id_12820  (alias)
 *   __libcgj_batch12820_marker = "libcgj-batch12820"
 *
 * Exclusive continuum CREATE-ONLY (12811-12820: proton soft all→0 —
 * proton_ok_u_12811, wine_ok_u_12812, proton_ge_ok_u_12813,
 * steam_runtime_ok_u_12814, pressure_vessel_ok_u_12815,
 * compat_tool_ok_u_12816, dxvk_ok_u_12817, vkd3d_ok_u_12818,
 * proton_soft_ready_u_12819, batch_id_12820→12820). Unique surfaces
 * only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * Distinct from gj_batch_id_12620 (batch12620), gj_batch_id_12420
 * (batch12420), and gj_batch_id_12220 (batch12220). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12820_marker[] = "libcgj-batch12820";

#define B12820_BATCH_ID  12820u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12820_id(void)
{
	return B12820_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_12820 - report this TU's graph batch number.
 *
 * Always returns 12820.
 */
uint32_t
gj_batch_id_12820(void)
{
	(void)NULL;
	return b12820_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_12820(void)
    __attribute__((alias("gj_batch_id_12820")));
