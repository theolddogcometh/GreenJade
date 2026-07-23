/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13221: Mesa soft id ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mesa_ok_u_13221(void);
 *     - Returns 0 (Mesa soft id stub not asserted ready). Soft
 *       compile-time product tag; not a live Mesa/libGL probe.
 *   uint32_t __gj_mesa_ok_u_13221  (alias)
 *   __libcgj_batch13221_marker = "libcgj-batch13221"
 *
 * Exclusive continuum CREATE-ONLY (13221-13230: mesa soft stubs —
 * all soft lamps →0; batch_id→13230 —
 * mesa_ok_u_13221, radv_ok_u_13222, amdvlk_ok_u_13223, anv_ok_u_13224,
 * lavapipe_ok_u_13225, zink_ok_u_13226, iris_ok_u_13227, v3d_ok_u_13228,
 * mesa_soft_ready_u_13229, batch_id_13230). Unique gj_mesa_ok_u_13221
 * surface only; no multi-def. Distinct from gj_mesa_ok_u_13021,
 * gj_mesa_ok_u_12821, gj_mesa_ok_u_12621, gj_mesa_ok_u_12421,
 * gj_mesa_ok_u_12221, gj_mesa_ok_u_12021, gj_mesa_ok_u_11921,
 * gj_mesa_ok_u_11721, gj_mesa_ok_u_11521, gj_mesa_ok_u_11321,
 * gj_mesa_ok_u_11121, gj_mesa_ok_u_10921, gj_mesa_ok_u_10721,
 * gj_mesa_ok_u_10521, gj_mesa_ok_u_10021 and prior mesa soft waves. No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13221_marker[] = "libcgj-batch13221";

/* Soft lamp: Mesa ok stub (not asserted). */
#define B13221_MESA_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13221_mesa_ok(void)
{
	return B13221_MESA_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mesa_ok_u_13221 - Mesa soft id ok stub.
 *
 * Always returns 0. Soft pure-data lamp; does not dlopen Mesa, query
 * GL/Vulkan ICDs, or probe driver paths. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_mesa_ok_u_13221(void)
{
	(void)NULL;
	return b13221_mesa_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mesa_ok_u_13221(void)
    __attribute__((alias("gj_mesa_ok_u_13221")));
