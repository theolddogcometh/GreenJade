/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12014: Steam Runtime ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_steam_runtime_ok_u_12014(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Steam Runtime probe for the proton/wine continuum.
 *   uint32_t __gj_steam_runtime_ok_u_12014  (alias)
 *   __libcgj_batch12014_marker = "libcgj-batch12014"
 *
 * Exclusive continuum CREATE-ONLY (12011-12020: proton soft all→0 —
 * proton_ok_u_12011, wine_ok_u_12012, proton_ge_ok_u_12013,
 * steam_runtime_ok_u_12014, pressure_vessel_ok_u_12015,
 * compat_tool_ok_u_12016, dxvk_ok_u_12017, vkd3d_ok_u_12018,
 * proton_soft_ready_u_12019, batch_id_12020→12020). Unique surface
 * only; no multi-def. Distinct from gj_steam_runtime_ok_u_11914
 * (batch11914), gj_steam_runtime_ok_u_11714 (batch11714), and
 * gj_steam_runtime_ok_u_11514 (batch11514). No parent wires. No
 * __int128. No Steam Runtime implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12014_marker[] = "libcgj-batch12014";

/* Soft Steam-Runtime-ok lamp: always off (not a real Steam Runtime probe). */
#define B12014_STEAM_RUNTIME_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12014_steam_runtime_ok(void)
{
	return B12014_STEAM_RUNTIME_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steam_runtime_ok_u_12014 - Steam Runtime ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect Steam Runtime
 * or call libc. No parent wires.
 */
uint32_t
gj_steam_runtime_ok_u_12014(void)
{
	(void)NULL;
	return b12014_steam_runtime_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_steam_runtime_ok_u_12014(void)
    __attribute__((alias("gj_steam_runtime_ok_u_12014")));
