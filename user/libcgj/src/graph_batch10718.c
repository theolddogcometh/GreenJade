/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10718: vkd3d ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_vkd3d_ok_u_10718(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       vkd3d/D3D12→Vulkan probe for the proton/wine continuum.
 *   uint32_t __gj_vkd3d_ok_u_10718  (alias)
 *   __libcgj_batch10718_marker = "libcgj-batch10718"
 *
 * Exclusive continuum CREATE-ONLY (10711-10720: proton soft all→0 —
 * proton_ok_u_10711, wine_ok_u_10712, proton_ge_ok_u_10713,
 * steam_runtime_ok_u_10714, pressure_vessel_ok_u_10715,
 * compat_tool_ok_u_10716, dxvk_ok_u_10717, vkd3d_ok_u_10718,
 * proton_soft_ready_u_10719, batch_id_10720→10720). Unique surface
 * only; no multi-def. Distinct from gj_dxvk_ok_u_10717 (batch10717)
 * and gj_vkd3d_ok_u_10218 (batch10218). No parent wires. No __int128.
 * No vkd3d implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10718_marker[] = "libcgj-batch10718";

/* Soft vkd3d-ok lamp: always off (not a real vkd3d probe). */
#define B10718_VKD3D_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10718_vkd3d_ok(void)
{
	return B10718_VKD3D_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vkd3d_ok_u_10718 - vkd3d ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect vkd3d or
 * call libc. No parent wires.
 */
uint32_t
gj_vkd3d_ok_u_10718(void)
{
	(void)NULL;
	return b10718_vkd3d_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_vkd3d_ok_u_10718(void)
    __attribute__((alias("gj_vkd3d_ok_u_10718")));
