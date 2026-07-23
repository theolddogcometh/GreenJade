/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13012: Wine ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_wine_ok_u_13012(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Wine runtime probe for the proton soft continuum.
 *   uint32_t __gj_wine_ok_u_13012  (alias)
 *   __libcgj_batch13012_marker = "libcgj-batch13012"
 *
 * Exclusive continuum CREATE-ONLY (13011-13020: proton soft all→0
 * stubs — proton_ok_u_13011, wine_ok_u_13012, proton_ge_ok_u_13013,
 * steam_runtime_ok_u_13014, pressure_vessel_ok_u_13015,
 * compat_tool_ok_u_13016, dxvk_ok_u_13017, vkd3d_ok_u_13018,
 * proton_soft_ready_u_13019, batch_id_13020). Unique surface only; no
 * multi-def. Distinct from gj_proton_ok_u_13011 (batch13011) and
 * gj_wine_ok_u_10312 / gj_wine_ok_u_10212. No parent wires. No
 * __int128. No Wine implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13012_marker[] = "libcgj-batch13012";

/* Soft Wine-ok lamp: always off (not a real Wine probe). */
#define B13012_WINE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13012_wine_ok(void)
{
	return B13012_WINE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wine_ok_u_13012 - Wine ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect Wine or
 * call libc. No parent wires.
 */
uint32_t
gj_wine_ok_u_13012(void)
{
	(void)NULL;
	return b13012_wine_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wine_ok_u_13012(void)
    __attribute__((alias("gj_wine_ok_u_13012")));
