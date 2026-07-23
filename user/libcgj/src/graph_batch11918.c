/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11918: vkd3d ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_vkd3d_ok_u_11918(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       vkd3d/D3D12→Vulkan probe for the proton/wine continuum.
 *   uint32_t __gj_vkd3d_ok_u_11918  (alias)
 *   __libcgj_batch11918_marker = "libcgj-batch11918"
 *
 * Exclusive continuum CREATE-ONLY (11911-11920: proton soft all→0 —
 * proton_ok_u_11911, wine_ok_u_11912, proton_ge_ok_u_11913,
 * steam_runtime_ok_u_11914, pressure_vessel_ok_u_11915,
 * compat_tool_ok_u_11916, dxvk_ok_u_11917, vkd3d_ok_u_11918,
 * proton_soft_ready_u_11919, batch_id_11920→11920). Unique surface
 * only; no multi-def. Distinct from gj_dxvk_ok_u_11917 (batch11917),
 * gj_vkd3d_ok_u_11718 (batch11718), gj_vkd3d_ok_u_11518 (batch11518),
 * and gj_vkd3d_ok_u_11318 (batch11318). No parent wires. No __int128.
 * No vkd3d implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11918_marker[] = "libcgj-batch11918";

/* Soft vkd3d-ok lamp: always off (not a real vkd3d probe). */
#define B11918_VKD3D_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11918_vkd3d_ok(void)
{
	return B11918_VKD3D_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vkd3d_ok_u_11918 - vkd3d ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect vkd3d or
 * call libc. No parent wires.
 */
uint32_t
gj_vkd3d_ok_u_11918(void)
{
	(void)NULL;
	return b11918_vkd3d_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_vkd3d_ok_u_11918(void)
    __attribute__((alias("gj_vkd3d_ok_u_11918")));
