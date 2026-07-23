/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13811: Proton ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_proton_ok_u_13811(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Proton/Wine runtime probe for the proton soft continuum.
 *   uint32_t __gj_proton_ok_u_13811  (alias)
 *   __libcgj_batch13811_marker = "libcgj-batch13811"
 *
 * Exclusive continuum CREATE-ONLY (13811-13820: proton soft all→0 —
 * proton_ok_u_13811, wine_ok_u_13812, proton_ge_ok_u_13813,
 * steam_runtime_ok_u_13814, pressure_vessel_ok_u_13815,
 * compat_tool_ok_u_13816, dxvk_ok_u_13817, vkd3d_ok_u_13818,
 * proton_soft_ready_u_13819, batch_id_13820→13820). Unique surface
 * only; no multi-def. Distinct from gj_proton_ok_u_13611 (batch13611),
 * gj_proton_ok_u_13411 (batch13411), gj_proton_ok_u_13211
 * (batch13211), and gj_proton_ok_u_13011 (batch13011). No parent
 * wires. No __int128. No Proton/Wine implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13811_marker[] = "libcgj-batch13811";

/* Soft Proton-ok lamp: always off (not a real Proton probe). */
#define B13811_PROTON_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13811_proton_ok(void)
{
	return B13811_PROTON_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_proton_ok_u_13811 - Proton ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect Proton or
 * call libc. No parent wires.
 */
uint32_t
gj_proton_ok_u_13811(void)
{
	(void)NULL;
	return b13811_proton_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_proton_ok_u_13811(void)
    __attribute__((alias("gj_proton_ok_u_13811")));
