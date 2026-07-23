/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11118: vkd3d ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_vkd3d_ok_u_11118(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       vkd3d/D3D12→Vulkan probe for the proton/wine continuum.
 *   uint32_t __gj_vkd3d_ok_u_11118  (alias)
 *   __libcgj_batch11118_marker = "libcgj-batch11118"
 *
 * Exclusive continuum CREATE-ONLY (11111-11120: proton soft all→0 —
 * proton_ok_u_11111, wine_ok_u_11112, proton_ge_ok_u_11113,
 * steam_runtime_ok_u_11114, pressure_vessel_ok_u_11115,
 * compat_tool_ok_u_11116, dxvk_ok_u_11117, vkd3d_ok_u_11118,
 * proton_soft_ready_u_11119, batch_id_11120→11120). Unique surface
 * only; no multi-def. Distinct from gj_dxvk_ok_u_11117 (batch11117),
 * gj_vkd3d_ok_u_10918 (batch10918), and gj_vkd3d_ok_u_10718
 * (batch10718). No parent wires. No __int128. No vkd3d implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11118_marker[] = "libcgj-batch11118";

/* Soft vkd3d-ok lamp: always off (not a real vkd3d probe). */
#define B11118_VKD3D_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11118_vkd3d_ok(void)
{
	return B11118_VKD3D_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vkd3d_ok_u_11118 - vkd3d ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect vkd3d or
 * call libc. No parent wires.
 */
uint32_t
gj_vkd3d_ok_u_11118(void)
{
	(void)NULL;
	return b11118_vkd3d_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_vkd3d_ok_u_11118(void)
    __attribute__((alias("gj_vkd3d_ok_u_11118")));
