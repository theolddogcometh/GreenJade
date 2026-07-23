/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13817: DXVK ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dxvk_ok_u_13817(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       DXVK/D3D→Vulkan probe for the proton soft continuum.
 *   uint32_t __gj_dxvk_ok_u_13817  (alias)
 *   __libcgj_batch13817_marker = "libcgj-batch13817"
 *
 * Exclusive continuum CREATE-ONLY (13811-13820: proton soft all→0 —
 * proton_ok_u_13811, wine_ok_u_13812, proton_ge_ok_u_13813,
 * steam_runtime_ok_u_13814, pressure_vessel_ok_u_13815,
 * compat_tool_ok_u_13816, dxvk_ok_u_13817, vkd3d_ok_u_13818,
 * proton_soft_ready_u_13819, batch_id_13820→13820). Unique surface
 * only; no multi-def. Distinct from gj_dxvk_ok_u_13617 (batch13617),
 * gj_dxvk_ok_u_13417 (batch13417), gj_dxvk_ok_u_13217 (batch13217),
 * and gj_dxvk_ok_u_13017 (batch13017). No parent wires. No __int128.
 * No DXVK implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13817_marker[] = "libcgj-batch13817";

/* Soft DXVK-ok lamp: always off (not a real DXVK probe). */
#define B13817_DXVK_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13817_dxvk_ok(void)
{
	return B13817_DXVK_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dxvk_ok_u_13817 - DXVK ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect DXVK or
 * call libc. No parent wires.
 */
uint32_t
gj_dxvk_ok_u_13817(void)
{
	(void)NULL;
	return b13817_dxvk_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dxvk_ok_u_13817(void)
    __attribute__((alias("gj_dxvk_ok_u_13817")));
