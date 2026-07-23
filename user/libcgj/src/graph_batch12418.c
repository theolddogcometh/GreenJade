/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12418: vkd3d ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_vkd3d_ok_u_12418(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       vkd3d/D3D12→Vulkan probe for the proton soft continuum.
 *   uint32_t __gj_vkd3d_ok_u_12418  (alias)
 *   __libcgj_batch12418_marker = "libcgj-batch12418"
 *
 * Exclusive continuum CREATE-ONLY (12411-12420: proton soft all→0 —
 * proton_ok_u_12411, wine_ok_u_12412, proton_ge_ok_u_12413,
 * steam_runtime_ok_u_12414, pressure_vessel_ok_u_12415,
 * compat_tool_ok_u_12416, dxvk_ok_u_12417, vkd3d_ok_u_12418,
 * proton_soft_ready_u_12419, batch_id_12420→12420). Unique surface
 * only; no multi-def. Distinct from gj_vkd3d_ok_u_12218 / _10218 /
 * _10013 and gj_dxvk_ok_u_12417 (batch12417). No parent wires. No
 * __int128. No vkd3d implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12418_marker[] = "libcgj-batch12418";

/* Soft vkd3d-ok lamp: always off (not a real vkd3d probe). */
#define B12418_VKD3D_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12418_vkd3d_ok(void)
{
	return B12418_VKD3D_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vkd3d_ok_u_12418 - vkd3d ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect vkd3d or
 * call libc. No parent wires.
 */
uint32_t
gj_vkd3d_ok_u_12418(void)
{
	(void)NULL;
	return b12418_vkd3d_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_vkd3d_ok_u_12418(void)
    __attribute__((alias("gj_vkd3d_ok_u_12418")));
