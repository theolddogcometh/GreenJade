/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13814: Steam Runtime ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_steam_runtime_ok_u_13814(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Steam Runtime probe for the proton soft continuum.
 *   uint32_t __gj_steam_runtime_ok_u_13814  (alias)
 *   __libcgj_batch13814_marker = "libcgj-batch13814"
 *
 * Exclusive continuum CREATE-ONLY (13811-13820: proton soft all→0 —
 * proton_ok_u_13811, wine_ok_u_13812, proton_ge_ok_u_13813,
 * steam_runtime_ok_u_13814, pressure_vessel_ok_u_13815,
 * compat_tool_ok_u_13816, dxvk_ok_u_13817, vkd3d_ok_u_13818,
 * proton_soft_ready_u_13819, batch_id_13820→13820). Unique surface
 * only; no multi-def. Distinct from gj_steam_runtime_ok_u_13614
 * (batch13614), gj_steam_runtime_ok_u_13414 (batch13414),
 * gj_steam_runtime_ok_u_13214 (batch13214), and
 * gj_steam_runtime_ok_u_13014 (batch13014). No parent wires. No
 * __int128. No Steam Runtime implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13814_marker[] = "libcgj-batch13814";

/* Soft Steam Runtime-ok lamp: always off (not a real runtime probe). */
#define B13814_STEAM_RUNTIME_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13814_steam_runtime_ok(void)
{
	return B13814_STEAM_RUNTIME_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steam_runtime_ok_u_13814 - Steam Runtime ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect Steam Runtime
 * or call libc. No parent wires.
 */
uint32_t
gj_steam_runtime_ok_u_13814(void)
{
	(void)NULL;
	return b13814_steam_runtime_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_steam_runtime_ok_u_13814(void)
    __attribute__((alias("gj_steam_runtime_ok_u_13814")));
