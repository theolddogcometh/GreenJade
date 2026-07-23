/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12615: pressure-vessel ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pressure_vessel_ok_u_12615(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       pressure-vessel / container probe for the proton soft continuum.
 *   uint32_t __gj_pressure_vessel_ok_u_12615  (alias)
 *   __libcgj_batch12615_marker = "libcgj-batch12615"
 *
 * Exclusive continuum CREATE-ONLY (12611-12620: proton soft all→0 —
 * proton_ok_u_12611, wine_ok_u_12612, proton_ge_ok_u_12613,
 * steam_runtime_ok_u_12614, pressure_vessel_ok_u_12615,
 * compat_tool_ok_u_12616, dxvk_ok_u_12617, vkd3d_ok_u_12618,
 * proton_soft_ready_u_12619, batch_id_12620→12620). Unique surface
 * only; no multi-def. Distinct from gj_pressure_vessel_ok_u_12215
 * (batch12215), gj_pressure_vessel_ok_u_12015 (batch12015), and
 * gj_pressure_vessel_ok_u_10215 (batch10215). No parent wires. No
 * __int128. No pressure-vessel implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12615_marker[] = "libcgj-batch12615";

/* Soft pressure-vessel-ok lamp: always off (not a real container probe). */
#define B12615_PRESSURE_VESSEL_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12615_pressure_vessel_ok(void)
{
	return B12615_PRESSURE_VESSEL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pressure_vessel_ok_u_12615 - pressure-vessel ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect pressure-vessel
 * or call libc. No parent wires.
 */
uint32_t
gj_pressure_vessel_ok_u_12615(void)
{
	(void)NULL;
	return b12615_pressure_vessel_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pressure_vessel_ok_u_12615(void)
    __attribute__((alias("gj_pressure_vessel_ok_u_12615")));
