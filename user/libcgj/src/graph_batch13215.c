/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13215: pressure-vessel ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pressure_vessel_ok_u_13215(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       pressure-vessel probe for the proton soft continuum.
 *   uint32_t __gj_pressure_vessel_ok_u_13215  (alias)
 *   __libcgj_batch13215_marker = "libcgj-batch13215"
 *
 * Exclusive continuum CREATE-ONLY (13211-13220: proton soft stubs —
 * ok slots all→0; proton_soft_ready→0; batch_id→13220 —
 * proton_ok_u_13211, wine_ok_u_13212, proton_ge_ok_u_13213,
 * steam_runtime_ok_u_13214, pressure_vessel_ok_u_13215,
 * compat_tool_ok_u_13216, dxvk_ok_u_13217, vkd3d_ok_u_13218,
 * proton_soft_ready_u_13219, batch_id_13220). Unique surface only;
 * no multi-def. Distinct from gj_pressure_vessel_ok_u_10515
 * (batch10515) and gj_pressure_vessel_ok_u_10215 (batch10215). No
 * parent wires. No __int128. No pressure-vessel implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13215_marker[] = "libcgj-batch13215";

/* Soft pressure-vessel-ok lamp: always off (not a real runtime probe). */
#define B13215_PRESSURE_VESSEL_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13215_ok(void)
{
	return B13215_PRESSURE_VESSEL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pressure_vessel_ok_u_13215 - pressure-vessel ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe pressure-vessel
 * presence or call libc. No parent wires.
 */
uint32_t
gj_pressure_vessel_ok_u_13215(void)
{
	(void)NULL;
	return b13215_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pressure_vessel_ok_u_13215(void)
    __attribute__((alias("gj_pressure_vessel_ok_u_13215")));
