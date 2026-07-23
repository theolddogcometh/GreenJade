/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12017: DXVK ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dxvk_ok_u_12017(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       DXVK/D3D→Vulkan probe for the proton/wine continuum.
 *   uint32_t __gj_dxvk_ok_u_12017  (alias)
 *   __libcgj_batch12017_marker = "libcgj-batch12017"
 *
 * Exclusive continuum CREATE-ONLY (12011-12020: proton soft all→0 —
 * proton_ok_u_12011, wine_ok_u_12012, proton_ge_ok_u_12013,
 * steam_runtime_ok_u_12014, pressure_vessel_ok_u_12015,
 * compat_tool_ok_u_12016, dxvk_ok_u_12017, vkd3d_ok_u_12018,
 * proton_soft_ready_u_12019, batch_id_12020→12020). Unique surface
 * only; no multi-def. Distinct from gj_dxvk_ok_u_11917 (batch11917),
 * gj_dxvk_ok_u_11717 (batch11717), and gj_dxvk_ok_u_11517
 * (batch11517). No parent wires. No __int128. No DXVK implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12017_marker[] = "libcgj-batch12017";

/* Soft DXVK-ok lamp: always off (not a real DXVK probe). */
#define B12017_DXVK_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12017_dxvk_ok(void)
{
	return B12017_DXVK_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dxvk_ok_u_12017 - DXVK ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect DXVK or
 * call libc. No parent wires.
 */
uint32_t
gj_dxvk_ok_u_12017(void)
{
	(void)NULL;
	return b12017_dxvk_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dxvk_ok_u_12017(void)
    __attribute__((alias("gj_dxvk_ok_u_12017")));
