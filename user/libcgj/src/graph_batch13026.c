/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13026: Zink soft id ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_zink_ok_u_13026(void);
 *     - Returns 0 (Zink soft id stub not asserted ready). Soft
 *       compile-time product tag; not a live zink GL-on-Vulkan probe.
 *   uint32_t __gj_zink_ok_u_13026  (alias)
 *   __libcgj_batch13026_marker = "libcgj-batch13026"
 *
 * Exclusive continuum CREATE-ONLY (13021-13030: mesa soft stubs —
 * all soft lamps →0; batch_id→13030). Unique gj_zink_ok_u_13026
 * surface only; no multi-def. Distinct from gj_zink_ok_u_12826,
 * gj_zink_ok_u_12626, gj_zink_ok_u_12426, gj_zink_ok_u_12226,
 * gj_zink_ok_u_12026, gj_zink_ok_u_11926, gj_zink_ok_u_11726,
 * gj_zink_ok_u_11526, gj_zink_ok_u_11326, gj_zink_ok_u_11126,
 * gj_zink_ok_u_10926, gj_zink_ok_u_10726, gj_zink_ok_u_10026 and sibling
 * mesa soft ok_u stubs in this wave. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13026_marker[] = "libcgj-batch13026";

/* Soft lamp: Zink ok stub (not asserted). */
#define B13026_ZINK_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13026_zink_ok(void)
{
	return B13026_ZINK_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_zink_ok_u_13026 - Zink soft id ok stub.
 *
 * Always returns 0. Soft pure-data lamp; does not load zink, open a
 * GL-on-Vulkan context, or probe driver paths. Does not call libc. No
 * parent wires.
 */
uint32_t
gj_zink_ok_u_13026(void)
{
	(void)NULL;
	return b13026_zink_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_zink_ok_u_13026(void)
    __attribute__((alias("gj_zink_ok_u_13026")));
