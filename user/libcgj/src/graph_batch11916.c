/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11916: Steam compat-tool ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_compat_tool_ok_u_11916(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Steam compatibility-tool probe for the proton/wine continuum.
 *   uint32_t __gj_compat_tool_ok_u_11916  (alias)
 *   __libcgj_batch11916_marker = "libcgj-batch11916"
 *
 * Exclusive continuum CREATE-ONLY (11911-11920: proton soft all→0 —
 * proton_ok_u_11911, wine_ok_u_11912, proton_ge_ok_u_11913,
 * steam_runtime_ok_u_11914, pressure_vessel_ok_u_11915,
 * compat_tool_ok_u_11916, dxvk_ok_u_11917, vkd3d_ok_u_11918,
 * proton_soft_ready_u_11919, batch_id_11920→11920). Unique surface
 * only; no multi-def. Distinct from gj_compat_tool_ok_u_11716
 * (batch11716), gj_compat_tool_ok_u_11516 (batch11516), and
 * gj_compat_tool_ok_u_11316 (batch11316). No parent wires. No
 * __int128. No Steam compat-tool implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11916_marker[] = "libcgj-batch11916";

/* Soft compat-tool-ok lamp: always off (not a real Steam compat-tool probe). */
#define B11916_COMPAT_TOOL_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11916_compat_tool_ok(void)
{
	return B11916_COMPAT_TOOL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_compat_tool_ok_u_11916 - Steam compat-tool ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect Steam
 * compatibility tools or call libc. No parent wires.
 */
uint32_t
gj_compat_tool_ok_u_11916(void)
{
	(void)NULL;
	return b11916_compat_tool_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_compat_tool_ok_u_11916(void)
    __attribute__((alias("gj_compat_tool_ok_u_11916")));
