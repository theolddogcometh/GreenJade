/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11112: Wine ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_wine_ok_u_11112(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Wine runtime probe for the proton/wine continuum.
 *   uint32_t __gj_wine_ok_u_11112  (alias)
 *   __libcgj_batch11112_marker = "libcgj-batch11112"
 *
 * Exclusive continuum CREATE-ONLY (11111-11120: proton soft all→0 —
 * proton_ok_u_11111, wine_ok_u_11112, proton_ge_ok_u_11113,
 * steam_runtime_ok_u_11114, pressure_vessel_ok_u_11115,
 * compat_tool_ok_u_11116, dxvk_ok_u_11117, vkd3d_ok_u_11118,
 * proton_soft_ready_u_11119, batch_id_11120→11120). Unique surface
 * only; no multi-def. Distinct from gj_proton_ok_u_11111 (batch11111),
 * gj_wine_ok_u_10912 (batch10912), and gj_wine_ok_u_10712 (batch10712).
 * No parent wires. No __int128. No Wine implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11112_marker[] = "libcgj-batch11112";

/* Soft Wine-ok lamp: always off (not a real Wine probe). */
#define B11112_WINE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11112_wine_ok(void)
{
	return B11112_WINE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wine_ok_u_11112 - Wine ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect Wine or
 * call libc. No parent wires.
 */
uint32_t
gj_wine_ok_u_11112(void)
{
	(void)NULL;
	return b11112_wine_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wine_ok_u_11112(void)
    __attribute__((alias("gj_wine_ok_u_11112")));
