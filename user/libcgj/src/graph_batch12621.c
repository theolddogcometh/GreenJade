/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12621: Mesa soft id ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mesa_ok_u_12621(void);
 *     - Returns 0 (Mesa soft id stub not asserted ready). Soft
 *       compile-time product tag; not a live Mesa/libGL probe.
 *   uint32_t __gj_mesa_ok_u_12621  (alias)
 *   __libcgj_batch12621_marker = "libcgj-batch12621"
 *
 * Exclusive continuum CREATE-ONLY (12621-12630: mesa soft stubs —
 * all soft lamps →0; batch_id→12630 —
 * mesa_ok_u_12621, radv_ok_u_12622, amdvlk_ok_u_12623, anv_ok_u_12624,
 * lavapipe_ok_u_12625, zink_ok_u_12626, iris_ok_u_12627, v3d_ok_u_12628,
 * mesa_soft_ready_u_12629, batch_id_12630). Unique gj_mesa_ok_u_12621
 * surface only; no multi-def. Distinct from gj_mesa_ok_u_12421,
 * gj_mesa_ok_u_12221, gj_mesa_ok_u_12021, gj_mesa_ok_u_11921,
 * gj_mesa_ok_u_11721, gj_mesa_ok_u_11521, gj_mesa_ok_u_11321,
 * gj_mesa_ok_u_11121, gj_mesa_ok_u_10921, gj_mesa_ok_u_10721,
 * gj_mesa_ok_u_10521, gj_mesa_ok_u_10021 and prior mesa soft waves.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12621_marker[] = "libcgj-batch12621";

/* Soft lamp: Mesa ok stub (not asserted). */
#define B12621_MESA_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12621_mesa_ok(void)
{
	return B12621_MESA_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mesa_ok_u_12621 - Mesa soft id ok stub.
 *
 * Always returns 0. Soft pure-data lamp; does not dlopen Mesa, query
 * GL/Vulkan ICDs, or probe driver paths. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_mesa_ok_u_12621(void)
{
	(void)NULL;
	return b12621_mesa_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mesa_ok_u_12621(void)
    __attribute__((alias("gj_mesa_ok_u_12621")));
