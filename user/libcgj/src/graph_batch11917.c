/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11917: DXVK ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dxvk_ok_u_11917(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       DXVK/D3D→Vulkan probe for the proton/wine continuum.
 *   uint32_t __gj_dxvk_ok_u_11917  (alias)
 *   __libcgj_batch11917_marker = "libcgj-batch11917"
 *
 * Exclusive continuum CREATE-ONLY (11911-11920: proton soft all→0 —
 * proton_ok_u_11911, wine_ok_u_11912, proton_ge_ok_u_11913,
 * steam_runtime_ok_u_11914, pressure_vessel_ok_u_11915,
 * compat_tool_ok_u_11916, dxvk_ok_u_11917, vkd3d_ok_u_11918,
 * proton_soft_ready_u_11919, batch_id_11920→11920). Unique surface
 * only; no multi-def. Distinct from gj_dxvk_ok_u_11717 (batch11717),
 * gj_dxvk_ok_u_11517 (batch11517), and gj_dxvk_ok_u_11317
 * (batch11317). No parent wires. No __int128. No DXVK implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11917_marker[] = "libcgj-batch11917";

/* Soft DXVK-ok lamp: always off (not a real DXVK probe). */
#define B11917_DXVK_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11917_dxvk_ok(void)
{
	return B11917_DXVK_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dxvk_ok_u_11917 - DXVK ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect DXVK or
 * call libc. No parent wires.
 */
uint32_t
gj_dxvk_ok_u_11917(void)
{
	(void)NULL;
	return b11917_dxvk_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dxvk_ok_u_11917(void)
    __attribute__((alias("gj_dxvk_ok_u_11917")));
