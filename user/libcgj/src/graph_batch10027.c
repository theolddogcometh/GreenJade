/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10027: Iris soft id ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_iris_ok_u_10027(void);
 *     - Returns 0 (Iris soft id stub not asserted ready). Soft
 *       compile-time product tag; not a live iris/Intel Gallium probe.
 *   uint32_t __gj_iris_ok_u_10027  (alias)
 *   __libcgj_batch10027_marker = "libcgj-batch10027"
 *
 * Exclusive CREATE-ONLY (10021-10030: mesa/radv soft id stubs). Unique
 * gj_iris_ok_u_10027 surface only; no multi-def. Distinct from
 * gj_mesa_ok_u_10021, gj_anv_ok_u_10024, and gj_zink_ok_u_10026. No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10027_marker[] = "libcgj-batch10027";

/* Soft lamp: Iris ok stub (not asserted). */
#define B10027_IRIS_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10027_iris_ok(void)
{
	return B10027_IRIS_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_iris_ok_u_10027 - Iris soft id ok stub.
 *
 * Always returns 0. Soft pure-data lamp; does not load iris, open DRI
 * nodes, or probe Intel GPU paths. Does not call libc. No parent wires.
 */
uint32_t
gj_iris_ok_u_10027(void)
{
	(void)NULL;
	return b10027_iris_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_iris_ok_u_10027(void)
    __attribute__((alias("gj_iris_ok_u_10027")));
