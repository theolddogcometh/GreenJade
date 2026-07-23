/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13017: DXVK ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dxvk_ok_u_13017(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       DXVK/D3D→Vulkan probe for the proton soft continuum.
 *   uint32_t __gj_dxvk_ok_u_13017  (alias)
 *   __libcgj_batch13017_marker = "libcgj-batch13017"
 *
 * Exclusive continuum CREATE-ONLY (13011-13020: proton soft all→0
 * stubs — proton_ok_u_13011, wine_ok_u_13012, proton_ge_ok_u_13013,
 * steam_runtime_ok_u_13014, pressure_vessel_ok_u_13015,
 * compat_tool_ok_u_13016, dxvk_ok_u_13017, vkd3d_ok_u_13018,
 * proton_soft_ready_u_13019, batch_id_13020). Unique surface only; no
 * multi-def. Distinct from gj_dxvk_ok_u_10317 / gj_dxvk_ok_u_10217.
 * No parent wires. No __int128. No DXVK implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13017_marker[] = "libcgj-batch13017";

/* Soft DXVK-ok lamp: always off (not a real DXVK probe). */
#define B13017_DXVK_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13017_dxvk_ok(void)
{
	return B13017_DXVK_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dxvk_ok_u_13017 - DXVK ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect DXVK or
 * call libc. No parent wires.
 */
uint32_t
gj_dxvk_ok_u_13017(void)
{
	(void)NULL;
	return b13017_dxvk_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dxvk_ok_u_13017(void)
    __attribute__((alias("gj_dxvk_ok_u_13017")));
