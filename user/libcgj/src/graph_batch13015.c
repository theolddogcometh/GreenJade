/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13015: pressure-vessel ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pressure_vessel_ok_u_13015(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       pressure-vessel / container probe for the proton soft continuum.
 *   uint32_t __gj_pressure_vessel_ok_u_13015  (alias)
 *   __libcgj_batch13015_marker = "libcgj-batch13015"
 *
 * Exclusive continuum CREATE-ONLY (13011-13020: proton soft all→0
 * stubs — proton_ok_u_13011, wine_ok_u_13012, proton_ge_ok_u_13013,
 * steam_runtime_ok_u_13014, pressure_vessel_ok_u_13015,
 * compat_tool_ok_u_13016, dxvk_ok_u_13017, vkd3d_ok_u_13018,
 * proton_soft_ready_u_13019, batch_id_13020). Unique surface only; no
 * multi-def. Distinct from gj_pressure_vessel_ok_u_10315 /
 * gj_pressure_vessel_ok_u_10215. No parent wires. No __int128. No
 * pressure-vessel implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13015_marker[] = "libcgj-batch13015";

/* Soft pressure-vessel-ok lamp: always off (not a real container probe). */
#define B13015_PRESSURE_VESSEL_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13015_pressure_vessel_ok(void)
{
	return B13015_PRESSURE_VESSEL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pressure_vessel_ok_u_13015 - pressure-vessel ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect pressure-vessel
 * or call libc. No parent wires.
 */
uint32_t
gj_pressure_vessel_ok_u_13015(void)
{
	(void)NULL;
	return b13015_pressure_vessel_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pressure_vessel_ok_u_13015(void)
    __attribute__((alias("gj_pressure_vessel_ok_u_13015")));
