/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12333: xkbcommon soft identity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_xkbcommon_ok_u_12333(void);
 *     - Returns 0 (soft: xkbcommon not ready at compile time).
 *   uint32_t __gj_xkbcommon_ok_u_12333  (alias)
 *   __libcgj_batch12333_marker = "libcgj-batch12333"
 *
 * Exclusive CREATE-ONLY wave 12331-12340 (IM soft: ibus/fcitx soft id
 * stubs — ready→1 others 0). Unique gj_xkbcommon_ok_u_12333 surface
 * only; no multi-def. Distinct from gj_xkbcommon_ok_u_12133
 * (batch12133), gj_xkbcommon_ok_u_11833 (batch11833), and sibling IM
 * soft stubs in this wave. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12333_marker[] = "libcgj-batch12333";

/* Soft xkbcommon ok lamp: not ready at compile time. */
#define B12333_XKBCOMMON_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12333_ok(void)
{
	return B12333_XKBCOMMON_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xkbcommon_ok_u_12333 - soft xkbcommon ok lamp.
 *
 * Always returns 0 (not ready at compile time). Does not load
 * xkbcommon or call libc. No parent wires.
 */
uint32_t
gj_xkbcommon_ok_u_12333(void)
{
	(void)NULL;
	return b12333_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_xkbcommon_ok_u_12333(void)
    __attribute__((alias("gj_xkbcommon_ok_u_12333")));
