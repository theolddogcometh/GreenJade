/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10013: soft vkd3d ok unit (open).
 *
 * Surface (unique symbols):
 *   uint32_t gj_vkd3d_ok_u_10013(void);
 *     - Returns 0 (vkd3d soft ok not complete for this bar3-related
 *       continuum; still open). Soft pure-data readiness lamp; does
 *       not load d3d12 or probe Vulkan translation layers.
 *   uint32_t __gj_vkd3d_ok_u_10013  (alias)
 *   __libcgj_batch10013_marker = "libcgj-batch10013"
 *
 * Exclusive continuum CREATE-ONLY (10011-10020: proton/dxvk soft id
 * stubs — proton_ge_ok_u_10011, dxvk_ok_u_10012, vkd3d_ok_u_10013,
 * wine_prefix_ok_u_10014, steamrt_ok_u_10015, gamescope_ok_u_10016,
 * mangohud_ok_u_10017, proton_launch_ok_u_10018,
 * proton_soft_ready_u_10019, batch_id_10020). Unique surface only; no
 * multi-def. Distinct from gj_vkd3d_ready_u_8986. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10013_marker[] = "libcgj-batch10013";

/* vkd3d soft-ok lamp for continuum 10011-10020 (still open). */
#define B10013_VKD3D_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10013_ok(void)
{
	return B10013_VKD3D_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_vkd3d_ok_u_10013 - report vkd3d soft-ok lamp (open).
 *
 * Always returns 0 (not complete / still open). Soft pure-data only;
 * does not probe d3d12/vkd3d or call libc. No parent wires.
 */
uint32_t
gj_vkd3d_ok_u_10013(void)
{
	(void)NULL;
	return b10013_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_vkd3d_ok_u_10013(void)
    __attribute__((alias("gj_vkd3d_ok_u_10013")));
