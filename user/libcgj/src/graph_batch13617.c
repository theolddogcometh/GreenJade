/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13617: DXVK ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dxvk_ok_u_13617(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       DXVK/D3D→Vulkan probe for the proton soft continuum.
 *   uint32_t __gj_dxvk_ok_u_13617  (alias)
 *   __libcgj_batch13617_marker = "libcgj-batch13617"
 *
 * Exclusive continuum CREATE-ONLY (13611-13620: proton soft all→0 —
 * proton_ok_u_13611, wine_ok_u_13612, proton_ge_ok_u_13613,
 * steam_runtime_ok_u_13614, pressure_vessel_ok_u_13615,
 * compat_tool_ok_u_13616, dxvk_ok_u_13617, vkd3d_ok_u_13618,
 * proton_soft_ready_u_13619, batch_id_13620→13620). Unique surface
 * only; no multi-def. Distinct from gj_dxvk_ok_u_13417 (batch13417),
 * gj_dxvk_ok_u_13217 (batch13217), gj_dxvk_ok_u_13017 (batch13017),
 * and gj_dxvk_ok_u_12817 (batch12817). No parent wires. No __int128.
 * No DXVK implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13617_marker[] = "libcgj-batch13617";

/* Soft DXVK-ok lamp: always off (not a real DXVK probe). */
#define B13617_DXVK_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13617_dxvk_ok(void)
{
	return B13617_DXVK_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dxvk_ok_u_13617 - DXVK ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect DXVK or
 * call libc. No parent wires.
 */
uint32_t
gj_dxvk_ok_u_13617(void)
{
	(void)NULL;
	return b13617_dxvk_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dxvk_ok_u_13617(void)
    __attribute__((alias("gj_dxvk_ok_u_13617")));
