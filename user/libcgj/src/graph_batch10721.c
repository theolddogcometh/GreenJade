/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10721: Mesa soft id ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mesa_ok_u_10721(void);
 *     - Returns 0 (Mesa soft id stub not asserted ready). Soft
 *       compile-time product tag; not a live Mesa/libGL probe.
 *   uint32_t __gj_mesa_ok_u_10721  (alias)
 *   __libcgj_batch10721_marker = "libcgj-batch10721"
 *
 * Exclusive continuum CREATE-ONLY (10721-10730: mesa soft stubs —
 * ok slots all→0; mesa_soft_ready→1; batch_id→10730 —
 * mesa_ok_u_10721, radv_ok_u_10722, amdvlk_ok_u_10723, anv_ok_u_10724,
 * lavapipe_ok_u_10725, zink_ok_u_10726, iris_ok_u_10727, v3d_ok_u_10728,
 * mesa_soft_ready_u_10729, batch_id_10730). Unique gj_mesa_ok_u_10721
 * surface only; no multi-def. Distinct from gj_mesa_ok_u_10021 and
 * prior mesa soft waves. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10721_marker[] = "libcgj-batch10721";

/* Soft lamp: Mesa ok stub (not asserted). */
#define B10721_MESA_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10721_mesa_ok(void)
{
	return B10721_MESA_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mesa_ok_u_10721 - Mesa soft id ok stub.
 *
 * Always returns 0. Soft pure-data lamp; does not dlopen Mesa, query
 * GL/Vulkan ICDs, or probe driver paths. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_mesa_ok_u_10721(void)
{
	(void)NULL;
	return b10721_mesa_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mesa_ok_u_10721(void)
    __attribute__((alias("gj_mesa_ok_u_10721")));
