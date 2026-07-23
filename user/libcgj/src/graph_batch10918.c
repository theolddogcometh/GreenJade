/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10918: vkd3d ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_vkd3d_ok_u_10918(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       vkd3d/D3D12→Vulkan probe for the proton/wine continuum.
 *   uint32_t __gj_vkd3d_ok_u_10918  (alias)
 *   __libcgj_batch10918_marker = "libcgj-batch10918"
 *
 * Exclusive continuum CREATE-ONLY (10911-10920: proton soft all→0 —
 * proton_ok_u_10911, wine_ok_u_10912, proton_ge_ok_u_10913,
 * steam_runtime_ok_u_10914, pressure_vessel_ok_u_10915,
 * compat_tool_ok_u_10916, dxvk_ok_u_10917, vkd3d_ok_u_10918,
 * proton_soft_ready_u_10919, batch_id_10920→10920). Unique surface
 * only; no multi-def. Distinct from gj_dxvk_ok_u_10917 (batch10917)
 * and gj_vkd3d_ok_u_10718 (batch10718). No parent wires. No __int128.
 * No vkd3d implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10918_marker[] = "libcgj-batch10918";

/* Soft vkd3d-ok lamp: always off (not a real vkd3d probe). */
#define B10918_VKD3D_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10918_vkd3d_ok(void)
{
	return B10918_VKD3D_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vkd3d_ok_u_10918 - vkd3d ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect vkd3d or
 * call libc. No parent wires.
 */
uint32_t
gj_vkd3d_ok_u_10918(void)
{
	(void)NULL;
	return b10918_vkd3d_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_vkd3d_ok_u_10918(void)
    __attribute__((alias("gj_vkd3d_ok_u_10918")));
