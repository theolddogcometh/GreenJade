/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10315: pressure-vessel ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pressure_vessel_ok_u_10315(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       pressure-vessel runtime probe for the proton/wine soft continuum.
 *   uint32_t __gj_pressure_vessel_ok_u_10315  (alias)
 *   __libcgj_batch10315_marker = "libcgj-batch10315"
 *
 * Exclusive continuum CREATE-ONLY (10311-10320: proton/wine soft
 * id stubs — proton_ok_u_10311, wine_ok_u_10312,
 * proton_ge_ok_u_10313, steam_runtime_ok_u_10314,
 * pressure_vessel_ok_u_10315, compat_tool_ok_u_10316,
 * dxvk_ok_u_10317, vkd3d_ok_u_10318,
 * proton_soft_ready_u_10319, batch_id_10320). Unique surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10315_marker[] = "libcgj-batch10315";

/* Soft pressure-vessel-ok lamp: always off (not a real runtime probe). */
#define B10315_PRESSURE_VESSEL_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10315_ok(void)
{
	return B10315_PRESSURE_VESSEL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pressure_vessel_ok_u_10315 - pressure-vessel ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe pressure-vessel
 * presence or call libc. No parent wires.
 */
uint32_t
gj_pressure_vessel_ok_u_10315(void)
{
	(void)NULL;
	return b10315_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pressure_vessel_ok_u_10315(void)
    __attribute__((alias("gj_pressure_vessel_ok_u_10315")));
