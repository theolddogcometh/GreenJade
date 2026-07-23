/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10028: V3D soft id ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_v3d_ok_u_10028(void);
 *     - Returns 0 (V3D soft id stub not asserted ready). Soft
 *       compile-time product tag; not a live v3d/Broadcom Gallium
 *       probe.
 *   uint32_t __gj_v3d_ok_u_10028  (alias)
 *   __libcgj_batch10028_marker = "libcgj-batch10028"
 *
 * Exclusive CREATE-ONLY (10021-10030: mesa/radv soft id stubs). Unique
 * gj_v3d_ok_u_10028 surface only; no multi-def. Distinct from
 * gj_mesa_ok_u_10021 and gj_iris_ok_u_10027. No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10028_marker[] = "libcgj-batch10028";

/* Soft lamp: V3D ok stub (not asserted). */
#define B10028_V3D_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10028_v3d_ok(void)
{
	return B10028_V3D_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_v3d_ok_u_10028 - V3D soft id ok stub.
 *
 * Always returns 0. Soft pure-data lamp; does not load v3d, open DRM
 * nodes, or probe Broadcom GPU paths. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_v3d_ok_u_10028(void)
{
	(void)NULL;
	return b10028_v3d_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_v3d_ok_u_10028(void)
    __attribute__((alias("gj_v3d_ok_u_10028")));
