/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10215: pressure-vessel ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pressure_vessel_ok_u_10215(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       pressure-vessel / container probe for the proton/wine continuum.
 *   uint32_t __gj_pressure_vessel_ok_u_10215  (alias)
 *   __libcgj_batch10215_marker = "libcgj-batch10215"
 *
 * Exclusive continuum CREATE-ONLY (10211-10220: proton/wine soft product
 * stubs — proton_ok_u_10211, wine_ok_u_10212, proton_ge_ok_u_10213,
 * steam_runtime_ok_u_10214, pressure_vessel_ok_u_10215,
 * compat_tool_ok_u_10216, dxvk_ok_u_10217, vkd3d_ok_u_10218,
 * proton_soft_ready_u_10219, batch_id_10220). Unique surface only; no
 * multi-def. No parent wires. No __int128. No pressure-vessel
 * implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10215_marker[] = "libcgj-batch10215";

/* Soft pressure-vessel-ok lamp: always off (not a real container probe). */
#define B10215_PRESSURE_VESSEL_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10215_pressure_vessel_ok(void)
{
	return B10215_PRESSURE_VESSEL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pressure_vessel_ok_u_10215 - pressure-vessel ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect pressure-vessel
 * or call libc. No parent wires.
 */
uint32_t
gj_pressure_vessel_ok_u_10215(void)
{
	(void)NULL;
	return b10215_pressure_vessel_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pressure_vessel_ok_u_10215(void)
    __attribute__((alias("gj_pressure_vessel_ok_u_10215")));
