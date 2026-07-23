/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10026: Zink soft id ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_zink_ok_u_10026(void);
 *     - Returns 0 (Zink soft id stub not asserted ready). Soft
 *       compile-time product tag; not a live zink/OpenGL-on-Vulkan
 *       probe.
 *   uint32_t __gj_zink_ok_u_10026  (alias)
 *   __libcgj_batch10026_marker = "libcgj-batch10026"
 *
 * Exclusive CREATE-ONLY (10021-10030: mesa/radv soft id stubs). Unique
 * gj_zink_ok_u_10026 surface only; no multi-def. Distinct from
 * gj_mesa_ok_u_10021 and gj_lavapipe_ok_u_10025. No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10026_marker[] = "libcgj-batch10026";

/* Soft lamp: Zink ok stub (not asserted). */
#define B10026_ZINK_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10026_zink_ok(void)
{
	return B10026_ZINK_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_zink_ok_u_10026 - Zink soft id ok stub.
 *
 * Always returns 0. Soft pure-data lamp; does not load zink, create a
 * GL context, or probe driver paths. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_zink_ok_u_10026(void)
{
	(void)NULL;
	return b10026_zink_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_zink_ok_u_10026(void)
    __attribute__((alias("gj_zink_ok_u_10026")));
