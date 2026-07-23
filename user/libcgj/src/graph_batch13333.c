/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13333: xkbcommon soft identity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_xkbcommon_ok_u_13333(void);
 *     - Returns 0 (soft: xkbcommon not ready at compile time).
 *   uint32_t __gj_xkbcommon_ok_u_13333  (alias)
 *   __libcgj_batch13333_marker = "libcgj-batch13333"
 *
 * Exclusive CREATE-ONLY wave 13331-13340 (ibus/fcitx soft id stubs).
 * Unique gj_xkbcommon_ok_u_13333 surface only; no multi-def. No parent
 * wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13333_marker[] = "libcgj-batch13333";

/* Soft xkbcommon ok lamp: not ready at compile time. */
#define B13333_XKBCOMMON_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13333_ok(void)
{
	return B13333_XKBCOMMON_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xkbcommon_ok_u_13333 - soft xkbcommon ok lamp.
 *
 * Always returns 0 (not ready at compile time). Does not load
 * libxkbcommon or call libc. No parent wires.
 */
uint32_t
gj_xkbcommon_ok_u_13333(void)
{
	(void)NULL;
	return b13333_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_xkbcommon_ok_u_13333(void)
    __attribute__((alias("gj_xkbcommon_ok_u_13333")));
