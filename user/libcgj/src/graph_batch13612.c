/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13612: Wine ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_wine_ok_u_13612(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Wine runtime probe for the proton soft continuum.
 *   uint32_t __gj_wine_ok_u_13612  (alias)
 *   __libcgj_batch13612_marker = "libcgj-batch13612"
 *
 * Exclusive continuum CREATE-ONLY (13611-13620: proton soft all→0 —
 * proton_ok_u_13611, wine_ok_u_13612, proton_ge_ok_u_13613,
 * steam_runtime_ok_u_13614, pressure_vessel_ok_u_13615,
 * compat_tool_ok_u_13616, dxvk_ok_u_13617, vkd3d_ok_u_13618,
 * proton_soft_ready_u_13619, batch_id_13620→13620). Unique surface
 * only; no multi-def. Distinct from gj_wine_ok_u_13412 (batch13412),
 * gj_wine_ok_u_13212 (batch13212), gj_wine_ok_u_13012 (batch13012),
 * and gj_wine_ok_u_12812 (batch12812). No parent wires. No __int128.
 * No Wine implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13612_marker[] = "libcgj-batch13612";

/* Soft Wine-ok lamp: always off (not a real Wine probe). */
#define B13612_WINE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13612_wine_ok(void)
{
	return B13612_WINE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wine_ok_u_13612 - Wine ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect Wine or
 * call libc. No parent wires.
 */
uint32_t
gj_wine_ok_u_13612(void)
{
	(void)NULL;
	return b13612_wine_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wine_ok_u_13612(void)
    __attribute__((alias("gj_wine_ok_u_13612")));
