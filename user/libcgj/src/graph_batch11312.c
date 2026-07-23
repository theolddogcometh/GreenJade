/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11312: Wine ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_wine_ok_u_11312(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Wine runtime probe for the proton/wine continuum.
 *   uint32_t __gj_wine_ok_u_11312  (alias)
 *   __libcgj_batch11312_marker = "libcgj-batch11312"
 *
 * Exclusive continuum CREATE-ONLY (11311-11320: proton soft all→0 —
 * proton_ok_u_11311, wine_ok_u_11312, proton_ge_ok_u_11313,
 * steam_runtime_ok_u_11314, pressure_vessel_ok_u_11315,
 * compat_tool_ok_u_11316, dxvk_ok_u_11317, vkd3d_ok_u_11318,
 * proton_soft_ready_u_11319, batch_id_11320→11320). Unique surface
 * only; no multi-def. Distinct from gj_proton_ok_u_11311 (batch11311),
 * gj_wine_ok_u_11112 (batch11112), and gj_wine_ok_u_10912 (batch10912).
 * No parent wires. No __int128. No Wine implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11312_marker[] = "libcgj-batch11312";

/* Soft Wine-ok lamp: always off (not a real Wine probe). */
#define B11312_WINE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11312_wine_ok(void)
{
	return B11312_WINE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wine_ok_u_11312 - Wine ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect Wine or
 * call libc. No parent wires.
 */
uint32_t
gj_wine_ok_u_11312(void)
{
	(void)NULL;
	return b11312_wine_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wine_ok_u_11312(void)
    __attribute__((alias("gj_wine_ok_u_11312")));
