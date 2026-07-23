/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13819: proton soft continuum lamp (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_proton_soft_ready_u_13819(void);
 *     - Always returns 0. Soft lamp only; does not claim proton/wine/
 *       proton_ge/steam_runtime/pressure_vessel/compat_tool/dxvk/vkd3d
 *       ok for the continuum.
 *   uint32_t __gj_proton_soft_ready_u_13819  (alias)
 *   __libcgj_batch13819_marker = "libcgj-batch13819"
 *
 * Exclusive continuum CREATE-ONLY (13811-13820: proton soft all→0 —
 * proton_ok_u_13811, wine_ok_u_13812, proton_ge_ok_u_13813,
 * steam_runtime_ok_u_13814, pressure_vessel_ok_u_13815,
 * compat_tool_ok_u_13816, dxvk_ok_u_13817, vkd3d_ok_u_13818,
 * proton_soft_ready_u_13819, batch_id_13820→13820). Unique surface
 * only; no multi-def. Distinct from gj_proton_soft_ready_u_13619
 * (batch13619), gj_proton_soft_ready_u_13419 (batch13419),
 * gj_proton_soft_ready_u_13219 (batch13219), and
 * gj_proton_soft_ready_u_13019 (batch13019). proton/wine/proton_ge/
 * steam_runtime/pressure_vessel/compat_tool/dxvk/vkd3d ok units
 * remain 0. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13819_marker[] = "libcgj-batch13819";

/* Soft continuum lamp: off (soft-ready unit is intentionally 0). */
#define B13819_SOFT_READY  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13819_soft_ready(void)
{
	return B13819_SOFT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_proton_soft_ready_u_13819 - proton soft continuum ready lamp.
 *
 * Always returns 0. Soft pure-data product tag for exclusive wave
 * 13811-13820; does not claim proton/wine/proton_ge/steam_runtime/
 * pressure_vessel/compat_tool/dxvk/vkd3d ok and does not call libc.
 * No parent wires.
 */
uint32_t
gj_proton_soft_ready_u_13819(void)
{
	(void)NULL;
	return b13819_soft_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_proton_soft_ready_u_13819(void)
    __attribute__((alias("gj_proton_soft_ready_u_13819")));
