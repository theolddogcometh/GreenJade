/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11315: pressure-vessel ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pressure_vessel_ok_u_11315(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       pressure-vessel / container probe for the proton/wine continuum.
 *   uint32_t __gj_pressure_vessel_ok_u_11315  (alias)
 *   __libcgj_batch11315_marker = "libcgj-batch11315"
 *
 * Exclusive continuum CREATE-ONLY (11311-11320: proton soft all→0 —
 * proton_ok_u_11311, wine_ok_u_11312, proton_ge_ok_u_11313,
 * steam_runtime_ok_u_11314, pressure_vessel_ok_u_11315,
 * compat_tool_ok_u_11316, dxvk_ok_u_11317, vkd3d_ok_u_11318,
 * proton_soft_ready_u_11319, batch_id_11320→11320). Unique surface
 * only; no multi-def. Distinct from gj_pressure_vessel_ok_u_11115
 * (batch11115) and gj_pressure_vessel_ok_u_10915 (batch10915). No
 * parent wires. No __int128. No pressure-vessel implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11315_marker[] = "libcgj-batch11315";

/* Soft pressure-vessel-ok lamp: always off (not a real container probe). */
#define B11315_PRESSURE_VESSEL_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11315_pressure_vessel_ok(void)
{
	return B11315_PRESSURE_VESSEL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pressure_vessel_ok_u_11315 - pressure-vessel ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect pressure-vessel
 * or call libc. No parent wires.
 */
uint32_t
gj_pressure_vessel_ok_u_11315(void)
{
	(void)NULL;
	return b11315_pressure_vessel_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pressure_vessel_ok_u_11315(void)
    __attribute__((alias("gj_pressure_vessel_ok_u_11315")));
