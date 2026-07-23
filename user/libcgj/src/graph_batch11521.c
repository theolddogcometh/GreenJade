/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11521: Mesa soft id ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mesa_ok_u_11521(void);
 *     - Returns 0 (Mesa soft id stub not asserted ready). Soft
 *       compile-time product tag; not a live Mesa/libGL probe.
 *   uint32_t __gj_mesa_ok_u_11521  (alias)
 *   __libcgj_batch11521_marker = "libcgj-batch11521"
 *
 * Exclusive continuum CREATE-ONLY (11521-11530: mesa soft stubs —
 * all soft lamps →0; batch_id→11530 —
 * mesa_ok_u_11521, radv_ok_u_11522, amdvlk_ok_u_11523, anv_ok_u_11524,
 * lavapipe_ok_u_11525, zink_ok_u_11526, iris_ok_u_11527, v3d_ok_u_11528,
 * mesa_soft_ready_u_11529, batch_id_11530). Unique gj_mesa_ok_u_11521
 * surface only; no multi-def. Distinct from gj_mesa_ok_u_11321,
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

const char __libcgj_batch11521_marker[] = "libcgj-batch11521";

/* Soft lamp: Mesa ok stub (not asserted). */
#define B11521_MESA_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11521_mesa_ok(void)
{
	return B11521_MESA_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mesa_ok_u_11521 - Mesa soft id ok stub.
 *
 * Always returns 0. Soft pure-data lamp; does not dlopen Mesa, query
 * GL/Vulkan ICDs, or probe driver paths. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_mesa_ok_u_11521(void)
{
	(void)NULL;
	return b11521_mesa_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mesa_ok_u_11521(void)
    __attribute__((alias("gj_mesa_ok_u_11521")));
