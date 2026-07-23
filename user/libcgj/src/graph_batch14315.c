/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14315: pressure-vessel ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pressure_vessel_ok_u_14315(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       pressure-vessel probe for the proton soft continuum.
 *   uint32_t __gj_pressure_vessel_ok_u_14315  (alias)
 *   __libcgj_batch14315_marker = "libcgj-batch14315"
 *
 * Exclusive continuum CREATE-ONLY (14311-14320: proton soft stubs —
 * proton_ok_u_14311, wine_ok_u_14312, proton_ge_ok_u_14313,
 * steam_runtime_ok_u_14314, pressure_vessel_ok_u_14315,
 * compat_tool_ok_u_14316, dxvk_ok_u_14317, vkd3d_ok_u_14318,
 * proton_soft_ready_u_14319, batch_id_14320). Unique surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14315_marker[] = "libcgj-batch14315";

/* Soft pressure-vessel-ok lamp: always off (not a real runtime probe). */
#define B14315_PRESSURE_VESSEL_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14315_ok(void)
{
	return B14315_PRESSURE_VESSEL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pressure_vessel_ok_u_14315 - pressure-vessel ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe pressure-vessel
 * presence or call libc. No parent wires.
 */
uint32_t
gj_pressure_vessel_ok_u_14315(void)
{
	(void)NULL;
	return b14315_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pressure_vessel_ok_u_14315(void)
    __attribute__((alias("gj_pressure_vessel_ok_u_14315")));
