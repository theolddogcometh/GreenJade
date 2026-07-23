/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13618: vkd3d ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_vkd3d_ok_u_13618(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       vkd3d/D3D12→Vulkan probe for the proton soft continuum.
 *   uint32_t __gj_vkd3d_ok_u_13618  (alias)
 *   __libcgj_batch13618_marker = "libcgj-batch13618"
 *
 * Exclusive continuum CREATE-ONLY (13611-13620: proton soft all→0 —
 * proton_ok_u_13611, wine_ok_u_13612, proton_ge_ok_u_13613,
 * steam_runtime_ok_u_13614, pressure_vessel_ok_u_13615,
 * compat_tool_ok_u_13616, dxvk_ok_u_13617, vkd3d_ok_u_13618,
 * proton_soft_ready_u_13619, batch_id_13620→13620). Unique surface
 * only; no multi-def. Distinct from gj_vkd3d_ok_u_13418 (batch13418),
 * gj_vkd3d_ok_u_13218 (batch13218), gj_vkd3d_ok_u_13018 (batch13018),
 * and gj_vkd3d_ok_u_12818 (batch12818). No parent wires. No __int128.
 * No vkd3d implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13618_marker[] = "libcgj-batch13618";

/* Soft vkd3d-ok lamp: always off (not a real vkd3d probe). */
#define B13618_VKD3D_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13618_vkd3d_ok(void)
{
	return B13618_VKD3D_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vkd3d_ok_u_13618 - vkd3d ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect vkd3d or
 * call libc. No parent wires.
 */
uint32_t
gj_vkd3d_ok_u_13618(void)
{
	(void)NULL;
	return b13618_vkd3d_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_vkd3d_ok_u_13618(void)
    __attribute__((alias("gj_vkd3d_ok_u_13618")));
