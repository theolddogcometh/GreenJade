/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14318: vkd3d ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_vkd3d_ok_u_14318(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       VKD3D runtime probe for the proton soft continuum.
 *   uint32_t __gj_vkd3d_ok_u_14318  (alias)
 *   __libcgj_batch14318_marker = "libcgj-batch14318"
 *
 * Exclusive continuum CREATE-ONLY (14311-14320: proton soft stubs —
 * proton_ok_u_14311, wine_ok_u_14312, proton_ge_ok_u_14313,
 * steam_runtime_ok_u_14314, pressure_vessel_ok_u_14315,
 * compat_tool_ok_u_14316, dxvk_ok_u_14317, vkd3d_ok_u_14318,
 * proton_soft_ready_u_14319, batch_id_14320). Unique surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14318_marker[] = "libcgj-batch14318";

/* Soft vkd3d-ok lamp: always off (not a real runtime probe). */
#define B14318_VKD3D_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14318_ok(void)
{
	return B14318_VKD3D_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vkd3d_ok_u_14318 - vkd3d ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe VKD3D
 * presence or call libc. No parent wires.
 */
uint32_t
gj_vkd3d_ok_u_14318(void)
{
	(void)NULL;
	return b14318_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_vkd3d_ok_u_14318(void)
    __attribute__((alias("gj_vkd3d_ok_u_14318")));
