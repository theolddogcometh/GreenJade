/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13020: proton soft all→0 + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13020(void);
 *     - Returns the compile-time graph batch number for this TU (13020).
 *   uint32_t __gj_batch_id_13020  (alias)
 *   __libcgj_batch13020_marker = "libcgj-batch13020"
 *
 * Exclusive continuum CREATE-ONLY (13011-13020: proton soft all→0
 * stubs — proton_ok_u_13011, wine_ok_u_13012, proton_ge_ok_u_13013,
 * steam_runtime_ok_u_13014, pressure_vessel_ok_u_13015,
 * compat_tool_ok_u_13016, dxvk_ok_u_13017, vkd3d_ok_u_13018,
 * proton_soft_ready_u_13019, batch_id_13020). Unique surfaces only; no
 * multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols — avoid multi-def. Distinct from gj_batch_id_10320
 * / gj_batch_id_10220. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13020_marker[] = "libcgj-batch13020";

#define B13020_BATCH_ID  13020u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13020_id(void)
{
	return B13020_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13020 - report this TU's graph batch number.
 *
 * Always returns 13020.
 */
uint32_t
gj_batch_id_13020(void)
{
	(void)NULL;
	return b13020_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13020(void)
    __attribute__((alias("gj_batch_id_13020")));
