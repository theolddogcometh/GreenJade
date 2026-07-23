/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12617: DXVK ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dxvk_ok_u_12617(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       DXVK/D3D→Vulkan probe for the proton soft continuum.
 *   uint32_t __gj_dxvk_ok_u_12617  (alias)
 *   __libcgj_batch12617_marker = "libcgj-batch12617"
 *
 * Exclusive continuum CREATE-ONLY (12611-12620: proton soft all→0 —
 * proton_ok_u_12611, wine_ok_u_12612, proton_ge_ok_u_12613,
 * steam_runtime_ok_u_12614, pressure_vessel_ok_u_12615,
 * compat_tool_ok_u_12616, dxvk_ok_u_12617, vkd3d_ok_u_12618,
 * proton_soft_ready_u_12619, batch_id_12620→12620). Unique surface
 * only; no multi-def. Distinct from gj_dxvk_ok_u_12217 (batch12217),
 * gj_dxvk_ok_u_12017 (batch12017), and gj_dxvk_ok_u_10217 /
 * _10012. No parent wires. No __int128. No DXVK implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12617_marker[] = "libcgj-batch12617";

/* Soft DXVK-ok lamp: always off (not a real DXVK probe). */
#define B12617_DXVK_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12617_dxvk_ok(void)
{
	return B12617_DXVK_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dxvk_ok_u_12617 - DXVK ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect DXVK or
 * call libc. No parent wires.
 */
uint32_t
gj_dxvk_ok_u_12617(void)
{
	(void)NULL;
	return b12617_dxvk_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dxvk_ok_u_12617(void)
    __attribute__((alias("gj_dxvk_ok_u_12617")));
