/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10515: pressure-vessel ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pressure_vessel_ok_u_10515(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       pressure-vessel probe for the proton soft continuum.
 *   uint32_t __gj_pressure_vessel_ok_u_10515  (alias)
 *   __libcgj_batch10515_marker = "libcgj-batch10515"
 *
 * Exclusive continuum CREATE-ONLY (10511-10520: proton soft stubs —
 * proton_ok_u_10511, wine_ok_u_10512, proton_ge_ok_u_10513,
 * steam_runtime_ok_u_10514, pressure_vessel_ok_u_10515,
 * compat_tool_ok_u_10516, dxvk_ok_u_10517, vkd3d_ok_u_10518,
 * proton_soft_ready_u_10519, batch_id_10520). Unique surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10515_marker[] = "libcgj-batch10515";

/* Soft pressure-vessel-ok lamp: always off (not a real runtime probe). */
#define B10515_PRESSURE_VESSEL_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10515_ok(void)
{
	return B10515_PRESSURE_VESSEL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pressure_vessel_ok_u_10515 - pressure-vessel ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe pressure-vessel
 * presence or call libc. No parent wires.
 */
uint32_t
gj_pressure_vessel_ok_u_10515(void)
{
	(void)NULL;
	return b10515_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pressure_vessel_ok_u_10515(void)
    __attribute__((alias("gj_pressure_vessel_ok_u_10515")));
