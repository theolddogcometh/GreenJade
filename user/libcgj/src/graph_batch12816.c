/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12816: Steam compat-tool ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_compat_tool_ok_u_12816(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Steam compatibility-tool probe for the proton soft continuum.
 *   uint32_t __gj_compat_tool_ok_u_12816  (alias)
 *   __libcgj_batch12816_marker = "libcgj-batch12816"
 *
 * Exclusive continuum CREATE-ONLY (12811-12820: proton soft all→0 —
 * proton_ok_u_12811, wine_ok_u_12812, proton_ge_ok_u_12813,
 * steam_runtime_ok_u_12814, pressure_vessel_ok_u_12815,
 * compat_tool_ok_u_12816, dxvk_ok_u_12817, vkd3d_ok_u_12818,
 * proton_soft_ready_u_12819, batch_id_12820→12820). Unique surface
 * only; no multi-def. Distinct from gj_compat_tool_ok_u_12616
 * (batch12616), gj_compat_tool_ok_u_12416 (batch12416), and
 * gj_compat_tool_ok_u_12216 (batch12216). No parent wires. No
 * __int128. No Steam compat-tool implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12816_marker[] = "libcgj-batch12816";

/* Soft compat-tool-ok lamp: always off (not a real tool probe). */
#define B12816_COMPAT_TOOL_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12816_compat_tool_ok(void)
{
	return B12816_COMPAT_TOOL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_compat_tool_ok_u_12816 - Steam compat-tool ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect Steam
 * compatibility tools or call libc. No parent wires.
 */
uint32_t
gj_compat_tool_ok_u_12816(void)
{
	(void)NULL;
	return b12816_compat_tool_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_compat_tool_ok_u_12816(void)
    __attribute__((alias("gj_compat_tool_ok_u_12816")));
