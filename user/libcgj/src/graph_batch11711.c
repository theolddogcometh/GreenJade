/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11711: Proton ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_proton_ok_u_11711(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Proton/Wine runtime probe for the proton/wine continuum.
 *   uint32_t __gj_proton_ok_u_11711  (alias)
 *   __libcgj_batch11711_marker = "libcgj-batch11711"
 *
 * Exclusive continuum CREATE-ONLY (11711-11720: proton soft all→0 —
 * proton_ok_u_11711, wine_ok_u_11712, proton_ge_ok_u_11713,
 * steam_runtime_ok_u_11714, pressure_vessel_ok_u_11715,
 * compat_tool_ok_u_11716, dxvk_ok_u_11717, vkd3d_ok_u_11718,
 * proton_soft_ready_u_11719, batch_id_11720→11720). Unique surface
 * only; no multi-def. Distinct from gj_proton_ok_u_11511 (batch11511),
 * gj_proton_ok_u_11311 (batch11311), and gj_proton_ok_u_11111
 * (batch11111). No parent wires. No __int128. No Proton/Wine
 * implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11711_marker[] = "libcgj-batch11711";

/* Soft Proton-ok lamp: always off (not a real Proton probe). */
#define B11711_PROTON_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11711_proton_ok(void)
{
	return B11711_PROTON_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_proton_ok_u_11711 - Proton ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect Proton or
 * call libc. No parent wires.
 */
uint32_t
gj_proton_ok_u_11711(void)
{
	(void)NULL;
	return b11711_proton_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_proton_ok_u_11711(void)
    __attribute__((alias("gj_proton_ok_u_11711")));
