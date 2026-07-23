/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11121: Mesa soft id ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mesa_ok_u_11121(void);
 *     - Returns 0 (Mesa soft id stub not asserted ready). Soft
 *       compile-time product tag; not a live Mesa/libGL probe.
 *   uint32_t __gj_mesa_ok_u_11121  (alias)
 *   __libcgj_batch11121_marker = "libcgj-batch11121"
 *
 * Exclusive continuum CREATE-ONLY (11121-11130: mesa soft stubs —
 * all soft lamps →0; batch_id→11130 —
 * mesa_ok_u_11121, radv_ok_u_11122, amdvlk_ok_u_11123, anv_ok_u_11124,
 * lavapipe_ok_u_11125, zink_ok_u_11126, iris_ok_u_11127, v3d_ok_u_11128,
 * mesa_soft_ready_u_11129, batch_id_11130). Unique gj_mesa_ok_u_11121
 * surface only; no multi-def. Distinct from gj_mesa_ok_u_10921,
 * gj_mesa_ok_u_10721, gj_mesa_ok_u_10521, gj_mesa_ok_u_10021 and prior mesa soft waves.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11121_marker[] = "libcgj-batch11121";

/* Soft lamp: Mesa ok stub (not asserted). */
#define B11121_MESA_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11121_mesa_ok(void)
{
	return B11121_MESA_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mesa_ok_u_11121 - Mesa soft id ok stub.
 *
 * Always returns 0. Soft pure-data lamp; does not dlopen Mesa, query
 * GL/Vulkan ICDs, or probe driver paths. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_mesa_ok_u_11121(void)
{
	(void)NULL;
	return b11121_mesa_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mesa_ok_u_11121(void)
    __attribute__((alias("gj_mesa_ok_u_11121")));
