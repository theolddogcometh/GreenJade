/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11517: DXVK ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dxvk_ok_u_11517(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       DXVK/D3D→Vulkan probe for the proton/wine continuum.
 *   uint32_t __gj_dxvk_ok_u_11517  (alias)
 *   __libcgj_batch11517_marker = "libcgj-batch11517"
 *
 * Exclusive continuum CREATE-ONLY (11511-11520: proton soft all→0 —
 * proton_ok_u_11511, wine_ok_u_11512, proton_ge_ok_u_11513,
 * steam_runtime_ok_u_11514, pressure_vessel_ok_u_11515,
 * compat_tool_ok_u_11516, dxvk_ok_u_11517, vkd3d_ok_u_11518,
 * proton_soft_ready_u_11519, batch_id_11520→11520). Unique surface
 * only; no multi-def. Distinct from gj_dxvk_ok_u_11317 (batch11317)
 * and gj_dxvk_ok_u_11117 (batch11117). No parent wires. No __int128.
 * No DXVK implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11517_marker[] = "libcgj-batch11517";

/* Soft DXVK-ok lamp: always off (not a real DXVK probe). */
#define B11517_DXVK_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11517_dxvk_ok(void)
{
	return B11517_DXVK_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dxvk_ok_u_11517 - DXVK ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect DXVK or
 * call libc. No parent wires.
 */
uint32_t
gj_dxvk_ok_u_11517(void)
{
	(void)NULL;
	return b11517_dxvk_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dxvk_ok_u_11517(void)
    __attribute__((alias("gj_dxvk_ok_u_11517")));
