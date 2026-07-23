/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11717: DXVK ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dxvk_ok_u_11717(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       DXVK/D3D→Vulkan probe for the proton/wine continuum.
 *   uint32_t __gj_dxvk_ok_u_11717  (alias)
 *   __libcgj_batch11717_marker = "libcgj-batch11717"
 *
 * Exclusive continuum CREATE-ONLY (11711-11720: proton soft all→0 —
 * proton_ok_u_11711, wine_ok_u_11712, proton_ge_ok_u_11713,
 * steam_runtime_ok_u_11714, pressure_vessel_ok_u_11715,
 * compat_tool_ok_u_11716, dxvk_ok_u_11717, vkd3d_ok_u_11718,
 * proton_soft_ready_u_11719, batch_id_11720→11720). Unique surface
 * only; no multi-def. Distinct from gj_dxvk_ok_u_11517 (batch11517),
 * gj_dxvk_ok_u_11317 (batch11317), and gj_dxvk_ok_u_11117
 * (batch11117). No parent wires. No __int128. No DXVK implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11717_marker[] = "libcgj-batch11717";

/* Soft DXVK-ok lamp: always off (not a real DXVK probe). */
#define B11717_DXVK_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11717_dxvk_ok(void)
{
	return B11717_DXVK_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dxvk_ok_u_11717 - DXVK ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect DXVK or
 * call libc. No parent wires.
 */
uint32_t
gj_dxvk_ok_u_11717(void)
{
	(void)NULL;
	return b11717_dxvk_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dxvk_ok_u_11717(void)
    __attribute__((alias("gj_dxvk_ok_u_11717")));
