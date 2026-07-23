/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11314: Steam Runtime ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_steam_runtime_ok_u_11314(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Steam Runtime probe for the proton/wine continuum.
 *   uint32_t __gj_steam_runtime_ok_u_11314  (alias)
 *   __libcgj_batch11314_marker = "libcgj-batch11314"
 *
 * Exclusive continuum CREATE-ONLY (11311-11320: proton soft all→0 —
 * proton_ok_u_11311, wine_ok_u_11312, proton_ge_ok_u_11313,
 * steam_runtime_ok_u_11314, pressure_vessel_ok_u_11315,
 * compat_tool_ok_u_11316, dxvk_ok_u_11317, vkd3d_ok_u_11318,
 * proton_soft_ready_u_11319, batch_id_11320→11320). Unique surface
 * only; no multi-def. Distinct from gj_steam_runtime_ok_u_11114
 * (batch11114) and gj_steam_runtime_ok_u_10914 (batch10914). No
 * parent wires. No __int128. No Steam Runtime implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11314_marker[] = "libcgj-batch11314";

/* Soft Steam-Runtime-ok lamp: always off (not a real Steam Runtime probe). */
#define B11314_STEAM_RUNTIME_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11314_steam_runtime_ok(void)
{
	return B11314_STEAM_RUNTIME_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steam_runtime_ok_u_11314 - Steam Runtime ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect Steam Runtime
 * or call libc. No parent wires.
 */
uint32_t
gj_steam_runtime_ok_u_11314(void)
{
	(void)NULL;
	return b11314_steam_runtime_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_steam_runtime_ok_u_11314(void)
    __attribute__((alias("gj_steam_runtime_ok_u_11314")));
