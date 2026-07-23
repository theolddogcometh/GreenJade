/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11111: Proton ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_proton_ok_u_11111(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Proton/Wine runtime probe for the proton/wine continuum.
 *   uint32_t __gj_proton_ok_u_11111  (alias)
 *   __libcgj_batch11111_marker = "libcgj-batch11111"
 *
 * Exclusive continuum CREATE-ONLY (11111-11120: proton soft all→0 —
 * proton_ok_u_11111, wine_ok_u_11112, proton_ge_ok_u_11113,
 * steam_runtime_ok_u_11114, pressure_vessel_ok_u_11115,
 * compat_tool_ok_u_11116, dxvk_ok_u_11117, vkd3d_ok_u_11118,
 * proton_soft_ready_u_11119, batch_id_11120→11120). Unique surface
 * only; no multi-def. Distinct from gj_proton_ok_u_10911 (batch10911)
 * and gj_proton_ok_u_10711 (batch10711). No parent wires. No __int128.
 * No Proton/Wine implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11111_marker[] = "libcgj-batch11111";

/* Soft Proton-ok lamp: always off (not a real Proton probe). */
#define B11111_PROTON_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11111_proton_ok(void)
{
	return B11111_PROTON_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_proton_ok_u_11111 - Proton ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect Proton or
 * call libc. No parent wires.
 */
uint32_t
gj_proton_ok_u_11111(void)
{
	(void)NULL;
	return b11111_proton_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_proton_ok_u_11111(void)
    __attribute__((alias("gj_proton_ok_u_11111")));
