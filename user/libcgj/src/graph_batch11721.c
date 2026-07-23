/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11721: Mesa soft id ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mesa_ok_u_11721(void);
 *     - Returns 0 (Mesa soft id stub not asserted ready). Soft
 *       compile-time product tag; not a live Mesa/libGL probe.
 *   uint32_t __gj_mesa_ok_u_11721  (alias)
 *   __libcgj_batch11721_marker = "libcgj-batch11721"
 *
 * Exclusive continuum CREATE-ONLY (11721-11730: mesa soft stubs —
 * all soft lamps →0; batch_id→11730 —
 * mesa_ok_u_11721, radv_ok_u_11722, amdvlk_ok_u_11723, anv_ok_u_11724,
 * lavapipe_ok_u_11725, zink_ok_u_11726, iris_ok_u_11727, v3d_ok_u_11728,
 * mesa_soft_ready_u_11729, batch_id_11730). Unique gj_mesa_ok_u_11721
 * surface only; no multi-def. Distinct from gj_mesa_ok_u_11521,
 * gj_mesa_ok_u_11321, gj_mesa_ok_u_11121, gj_mesa_ok_u_10921,
 * gj_mesa_ok_u_10721, gj_mesa_ok_u_10521, gj_mesa_ok_u_10021 and prior
 * mesa soft waves. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11721_marker[] = "libcgj-batch11721";

/* Soft lamp: Mesa ok stub (not asserted). */
#define B11721_MESA_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11721_mesa_ok(void)
{
	return B11721_MESA_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mesa_ok_u_11721 - Mesa soft id ok stub.
 *
 * Always returns 0. Soft pure-data lamp; does not dlopen Mesa, query
 * GL/Vulkan ICDs, or probe driver paths. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_mesa_ok_u_11721(void)
{
	(void)NULL;
	return b11721_mesa_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mesa_ok_u_11721(void)
    __attribute__((alias("gj_mesa_ok_u_11721")));
