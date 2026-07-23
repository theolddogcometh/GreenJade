/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10921: Mesa soft id ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mesa_ok_u_10921(void);
 *     - Returns 0 (Mesa soft id stub not asserted ready). Soft
 *       compile-time product tag; not a live Mesa/libGL probe.
 *   uint32_t __gj_mesa_ok_u_10921  (alias)
 *   __libcgj_batch10921_marker = "libcgj-batch10921"
 *
 * Exclusive continuum CREATE-ONLY (10921-10930: mesa soft stubs —
 * all soft lamps →0; batch_id→10930 —
 * mesa_ok_u_10921, radv_ok_u_10922, amdvlk_ok_u_10923, anv_ok_u_10924,
 * lavapipe_ok_u_10925, zink_ok_u_10926, iris_ok_u_10927, v3d_ok_u_10928,
 * mesa_soft_ready_u_10929, batch_id_10930). Unique gj_mesa_ok_u_10921
 * surface only; no multi-def. Distinct from gj_mesa_ok_u_10721,
 * gj_mesa_ok_u_10521, gj_mesa_ok_u_10021 and prior mesa soft waves.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10921_marker[] = "libcgj-batch10921";

/* Soft lamp: Mesa ok stub (not asserted). */
#define B10921_MESA_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10921_mesa_ok(void)
{
	return B10921_MESA_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mesa_ok_u_10921 - Mesa soft id ok stub.
 *
 * Always returns 0. Soft pure-data lamp; does not dlopen Mesa, query
 * GL/Vulkan ICDs, or probe driver paths. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_mesa_ok_u_10921(void)
{
	(void)NULL;
	return b10921_mesa_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mesa_ok_u_10921(void)
    __attribute__((alias("gj_mesa_ok_u_10921")));
