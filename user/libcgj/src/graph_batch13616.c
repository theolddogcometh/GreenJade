/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13616: Steam compat-tool ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_compat_tool_ok_u_13616(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Steam compatibility-tool probe for the proton soft continuum.
 *   uint32_t __gj_compat_tool_ok_u_13616  (alias)
 *   __libcgj_batch13616_marker = "libcgj-batch13616"
 *
 * Exclusive continuum CREATE-ONLY (13611-13620: proton soft all→0 —
 * proton_ok_u_13611, wine_ok_u_13612, proton_ge_ok_u_13613,
 * steam_runtime_ok_u_13614, pressure_vessel_ok_u_13615,
 * compat_tool_ok_u_13616, dxvk_ok_u_13617, vkd3d_ok_u_13618,
 * proton_soft_ready_u_13619, batch_id_13620→13620). Unique surface
 * only; no multi-def. Distinct from gj_compat_tool_ok_u_13416
 * (batch13416), gj_compat_tool_ok_u_13216 (batch13216),
 * gj_compat_tool_ok_u_13016 (batch13016), and
 * gj_compat_tool_ok_u_12816 (batch12816). No parent wires. No
 * __int128. No Steam compat-tool implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13616_marker[] = "libcgj-batch13616";

/* Soft compat-tool-ok lamp: always off (not a real tool probe). */
#define B13616_COMPAT_TOOL_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13616_compat_tool_ok(void)
{
	return B13616_COMPAT_TOOL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_compat_tool_ok_u_13616 - Steam compat-tool ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect Steam
 * compatibility tools or call libc. No parent wires.
 */
uint32_t
gj_compat_tool_ok_u_13616(void)
{
	(void)NULL;
	return b13616_compat_tool_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_compat_tool_ok_u_13616(void)
    __attribute__((alias("gj_compat_tool_ok_u_13616")));
