/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11712: Wine ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_wine_ok_u_11712(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Wine runtime probe for the proton/wine continuum.
 *   uint32_t __gj_wine_ok_u_11712  (alias)
 *   __libcgj_batch11712_marker = "libcgj-batch11712"
 *
 * Exclusive continuum CREATE-ONLY (11711-11720: proton soft all→0 —
 * proton_ok_u_11711, wine_ok_u_11712, proton_ge_ok_u_11713,
 * steam_runtime_ok_u_11714, pressure_vessel_ok_u_11715,
 * compat_tool_ok_u_11716, dxvk_ok_u_11717, vkd3d_ok_u_11718,
 * proton_soft_ready_u_11719, batch_id_11720→11720). Unique surface
 * only; no multi-def. Distinct from gj_proton_ok_u_11711 (batch11711),
 * gj_wine_ok_u_11512 (batch11512), gj_wine_ok_u_11312 (batch11312),
 * and gj_wine_ok_u_11112 (batch11112). No parent wires. No __int128.
 * No Wine implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11712_marker[] = "libcgj-batch11712";

/* Soft Wine-ok lamp: always off (not a real Wine probe). */
#define B11712_WINE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11712_wine_ok(void)
{
	return B11712_WINE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wine_ok_u_11712 - Wine ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect Wine or
 * call libc. No parent wires.
 */
uint32_t
gj_wine_ok_u_11712(void)
{
	(void)NULL;
	return b11712_wine_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wine_ok_u_11712(void)
    __attribute__((alias("gj_wine_ok_u_11712")));
