/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12226: Zink soft id ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_zink_ok_u_12226(void);
 *     - Returns 0 (Zink soft id stub not asserted ready). Soft
 *       compile-time product tag; not a live zink OpenGL-on-Vulkan
 *       probe.
 *   uint32_t __gj_zink_ok_u_12226  (alias)
 *   __libcgj_batch12226_marker = "libcgj-batch12226"
 *
 * Exclusive continuum CREATE-ONLY (12221-12230: mesa soft stubs —
 * all soft lamps →0; batch_id→12230). Unique gj_zink_ok_u_12226
 * surface only; no multi-def. Distinct from gj_zink_ok_u_12026,
 * gj_zink_ok_u_11926, gj_zink_ok_u_11726, gj_zink_ok_u_11526,
 * gj_zink_ok_u_11326, gj_zink_ok_u_11126, gj_zink_ok_u_10926,
 * gj_zink_ok_u_10726, gj_zink_ok_u_10026 and sibling mesa soft ok_u
 * stubs in this wave. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12226_marker[] = "libcgj-batch12226";

/* Soft lamp: Zink ok stub (not asserted). */
#define B12226_ZINK_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12226_zink_ok(void)
{
	return B12226_ZINK_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_zink_ok_u_12226 - Zink soft id ok stub.
 *
 * Always returns 0. Soft pure-data lamp; does not load zink, create a
 * GL-on-Vulkan context, or probe driver paths. Does not call libc. No
 * parent wires.
 */
uint32_t
gj_zink_ok_u_12226(void)
{
	(void)NULL;
	return b12226_zink_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_zink_ok_u_12226(void)
    __attribute__((alias("gj_zink_ok_u_12226")));
