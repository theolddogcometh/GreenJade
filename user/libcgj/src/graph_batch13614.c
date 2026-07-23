/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13614: Steam Runtime ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_steam_runtime_ok_u_13614(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Steam Runtime probe for the proton soft continuum.
 *   uint32_t __gj_steam_runtime_ok_u_13614  (alias)
 *   __libcgj_batch13614_marker = "libcgj-batch13614"
 *
 * Exclusive continuum CREATE-ONLY (13611-13620: proton soft all→0 —
 * proton_ok_u_13611, wine_ok_u_13612, proton_ge_ok_u_13613,
 * steam_runtime_ok_u_13614, pressure_vessel_ok_u_13615,
 * compat_tool_ok_u_13616, dxvk_ok_u_13617, vkd3d_ok_u_13618,
 * proton_soft_ready_u_13619, batch_id_13620→13620). Unique surface
 * only; no multi-def. Distinct from gj_steam_runtime_ok_u_13414
 * (batch13414), gj_steam_runtime_ok_u_13214 (batch13214),
 * gj_steam_runtime_ok_u_13014 (batch13014), and
 * gj_steam_runtime_ok_u_12814 (batch12814). No parent wires. No
 * __int128. No Steam Runtime implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13614_marker[] = "libcgj-batch13614";

/* Soft Steam Runtime-ok lamp: always off (not a real runtime probe). */
#define B13614_STEAM_RUNTIME_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13614_steam_runtime_ok(void)
{
	return B13614_STEAM_RUNTIME_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steam_runtime_ok_u_13614 - Steam Runtime ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect Steam Runtime
 * or call libc. No parent wires.
 */
uint32_t
gj_steam_runtime_ok_u_13614(void)
{
	(void)NULL;
	return b13614_steam_runtime_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_steam_runtime_ok_u_13614(void)
    __attribute__((alias("gj_steam_runtime_ok_u_13614")));
