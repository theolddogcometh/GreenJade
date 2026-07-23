/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12012: Wine ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_wine_ok_u_12012(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Wine runtime probe for the proton/wine continuum.
 *   uint32_t __gj_wine_ok_u_12012  (alias)
 *   __libcgj_batch12012_marker = "libcgj-batch12012"
 *
 * Exclusive continuum CREATE-ONLY (12011-12020: proton soft all→0 —
 * proton_ok_u_12011, wine_ok_u_12012, proton_ge_ok_u_12013,
 * steam_runtime_ok_u_12014, pressure_vessel_ok_u_12015,
 * compat_tool_ok_u_12016, dxvk_ok_u_12017, vkd3d_ok_u_12018,
 * proton_soft_ready_u_12019, batch_id_12020→12020). Unique surface
 * only; no multi-def. Distinct from gj_proton_ok_u_12011 (batch12011),
 * gj_wine_ok_u_11912 (batch11912), gj_wine_ok_u_11712 (batch11712),
 * and gj_wine_ok_u_11512 (batch11512). No parent wires. No __int128.
 * No Wine implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12012_marker[] = "libcgj-batch12012";

/* Soft Wine-ok lamp: always off (not a real Wine probe). */
#define B12012_WINE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12012_wine_ok(void)
{
	return B12012_WINE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wine_ok_u_12012 - Wine ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect Wine or
 * call libc. No parent wires.
 */
uint32_t
gj_wine_ok_u_12012(void)
{
	(void)NULL;
	return b12012_wine_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wine_ok_u_12012(void)
    __attribute__((alias("gj_wine_ok_u_12012")));
