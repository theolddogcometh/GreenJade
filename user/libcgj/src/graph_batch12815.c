/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12815: pressure-vessel ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pressure_vessel_ok_u_12815(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       pressure-vessel / container probe for the proton soft continuum.
 *   uint32_t __gj_pressure_vessel_ok_u_12815  (alias)
 *   __libcgj_batch12815_marker = "libcgj-batch12815"
 *
 * Exclusive continuum CREATE-ONLY (12811-12820: proton soft all→0 —
 * proton_ok_u_12811, wine_ok_u_12812, proton_ge_ok_u_12813,
 * steam_runtime_ok_u_12814, pressure_vessel_ok_u_12815,
 * compat_tool_ok_u_12816, dxvk_ok_u_12817, vkd3d_ok_u_12818,
 * proton_soft_ready_u_12819, batch_id_12820→12820). Unique surface
 * only; no multi-def. Distinct from gj_pressure_vessel_ok_u_12615
 * (batch12615), gj_pressure_vessel_ok_u_12415 (batch12415), and
 * gj_pressure_vessel_ok_u_12215 (batch12215). No parent wires. No
 * __int128. No pressure-vessel implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12815_marker[] = "libcgj-batch12815";

/* Soft pressure-vessel-ok lamp: always off (not a real container probe). */
#define B12815_PRESSURE_VESSEL_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12815_pressure_vessel_ok(void)
{
	return B12815_PRESSURE_VESSEL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pressure_vessel_ok_u_12815 - pressure-vessel ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect pressure-vessel
 * or call libc. No parent wires.
 */
uint32_t
gj_pressure_vessel_ok_u_12815(void)
{
	(void)NULL;
	return b12815_pressure_vessel_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pressure_vessel_ok_u_12815(void)
    __attribute__((alias("gj_pressure_vessel_ok_u_12815")));
