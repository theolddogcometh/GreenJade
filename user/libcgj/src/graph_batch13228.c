/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13228: V3D soft id ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_v3d_ok_u_13228(void);
 *     - Returns 0 (V3D soft id stub not asserted ready). Soft
 *       compile-time product tag; not a live v3d/Broadcom DRM probe.
 *   uint32_t __gj_v3d_ok_u_13228  (alias)
 *   __libcgj_batch13228_marker = "libcgj-batch13228"
 *
 * Exclusive continuum CREATE-ONLY (13221-13230: mesa soft stubs —
 * all soft lamps →0; batch_id→13230). Unique gj_v3d_ok_u_13228
 * surface only; no multi-def. Distinct from gj_v3d_ok_u_13028,
 * gj_v3d_ok_u_12828, gj_v3d_ok_u_12628, gj_v3d_ok_u_12428,
 * gj_v3d_ok_u_12228, gj_v3d_ok_u_12028, gj_v3d_ok_u_11928,
 * gj_v3d_ok_u_11728, gj_v3d_ok_u_11528, gj_v3d_ok_u_11328,
 * gj_v3d_ok_u_11128, gj_v3d_ok_u_10928, gj_v3d_ok_u_10728,
 * gj_v3d_ok_u_10028 and sibling mesa soft ok_u stubs in this wave. No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13228_marker[] = "libcgj-batch13228";

/* Soft lamp: V3D ok stub (not asserted). */
#define B13228_V3D_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13228_v3d_ok(void)
{
	return B13228_V3D_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_v3d_ok_u_13228 - V3D soft id ok stub.
 *
 * Always returns 0. Soft pure-data lamp; does not load v3d, open a
 * Broadcom DRM device, or probe driver paths. Does not call libc. No
 * parent wires.
 */
uint32_t
gj_v3d_ok_u_13228(void)
{
	(void)NULL;
	return b13228_v3d_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_v3d_ok_u_13228(void)
    __attribute__((alias("gj_v3d_ok_u_13228")));
