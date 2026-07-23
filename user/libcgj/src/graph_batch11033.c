/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11033: xkbcommon soft identity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_xkbcommon_ok_u_11033(void);
 *     - Returns 0 (soft: xkbcommon not ready at compile time).
 *   uint32_t __gj_xkbcommon_ok_u_11033  (alias)
 *   __libcgj_batch11033_marker = "libcgj-batch11033"
 *
 * Exclusive CREATE-ONLY wave 11031-11040 (ibus/fcitx soft id stubs).
 * Unique gj_xkbcommon_ok_u_11033 surface only; no multi-def. No parent
 * wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11033_marker[] = "libcgj-batch11033";

/* Soft xkbcommon ok lamp: not ready at compile time. */
#define B11033_XKBCOMMON_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11033_ok(void)
{
	return B11033_XKBCOMMON_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xkbcommon_ok_u_11033 - soft xkbcommon ok lamp.
 *
 * Always returns 0 (not ready at compile time). Does not probe xkb
 * state or call libc. No parent wires.
 */
uint32_t
gj_xkbcommon_ok_u_11033(void)
{
	(void)NULL;
	return b11033_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_xkbcommon_ok_u_11033(void)
    __attribute__((alias("gj_xkbcommon_ok_u_11033")));
