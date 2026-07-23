/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10712: Wine ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_wine_ok_u_10712(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Wine runtime probe for the proton/wine continuum.
 *   uint32_t __gj_wine_ok_u_10712  (alias)
 *   __libcgj_batch10712_marker = "libcgj-batch10712"
 *
 * Exclusive continuum CREATE-ONLY (10711-10720: proton soft all→0 —
 * proton_ok_u_10711, wine_ok_u_10712, proton_ge_ok_u_10713,
 * steam_runtime_ok_u_10714, pressure_vessel_ok_u_10715,
 * compat_tool_ok_u_10716, dxvk_ok_u_10717, vkd3d_ok_u_10718,
 * proton_soft_ready_u_10719, batch_id_10720→10720). Unique surface
 * only; no multi-def. Distinct from gj_proton_ok_u_10711 (batch10711)
 * and gj_wine_ok_u_10212 (batch10212). No parent wires. No __int128.
 * No Wine implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10712_marker[] = "libcgj-batch10712";

/* Soft Wine-ok lamp: always off (not a real Wine probe). */
#define B10712_WINE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10712_wine_ok(void)
{
	return B10712_WINE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wine_ok_u_10712 - Wine ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect Wine or
 * call libc. No parent wires.
 */
uint32_t
gj_wine_ok_u_10712(void)
{
	(void)NULL;
	return b10712_wine_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wine_ok_u_10712(void)
    __attribute__((alias("gj_wine_ok_u_10712")));
