/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12812: Wine ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_wine_ok_u_12812(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Wine runtime probe for the proton soft continuum.
 *   uint32_t __gj_wine_ok_u_12812  (alias)
 *   __libcgj_batch12812_marker = "libcgj-batch12812"
 *
 * Exclusive continuum CREATE-ONLY (12811-12820: proton soft all→0 —
 * proton_ok_u_12811, wine_ok_u_12812, proton_ge_ok_u_12813,
 * steam_runtime_ok_u_12814, pressure_vessel_ok_u_12815,
 * compat_tool_ok_u_12816, dxvk_ok_u_12817, vkd3d_ok_u_12818,
 * proton_soft_ready_u_12819, batch_id_12820→12820). Unique surface
 * only; no multi-def. Distinct from gj_wine_ok_u_12612 (batch12612),
 * gj_wine_ok_u_12412 (batch12412), and gj_wine_ok_u_12212
 * (batch12212). No parent wires. No __int128. No Wine implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12812_marker[] = "libcgj-batch12812";

/* Soft Wine-ok lamp: always off (not a real Wine probe). */
#define B12812_WINE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12812_wine_ok(void)
{
	return B12812_WINE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wine_ok_u_12812 - Wine ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect Wine or
 * call libc. No parent wires.
 */
uint32_t
gj_wine_ok_u_12812(void)
{
	(void)NULL;
	return b12812_wine_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wine_ok_u_12812(void)
    __attribute__((alias("gj_wine_ok_u_12812")));
