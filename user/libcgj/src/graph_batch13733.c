/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13733: xkbcommon soft identity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_xkbcommon_ok_u_13733(void);
 *     - Returns 0 (soft: xkbcommon not ready at compile time).
 *   uint32_t __gj_xkbcommon_ok_u_13733  (alias)
 *   __libcgj_batch13733_marker = "libcgj-batch13733"
 *
 * Exclusive CREATE-ONLY wave 13731-13740 (ibus/fcitx soft id stubs).
 * Unique gj_xkbcommon_ok_u_13733 surface only; no multi-def. No parent
 * wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13733_marker[] = "libcgj-batch13733";

/* Soft xkbcommon ok lamp: not ready at compile time. */
#define B13733_XKBCOMMON_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13733_ok(void)
{
	return B13733_XKBCOMMON_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xkbcommon_ok_u_13733 - soft xkbcommon ok lamp.
 *
 * Always returns 0 (not ready at compile time). Does not load
 * libxkbcommon or call libc. No parent wires.
 */
uint32_t
gj_xkbcommon_ok_u_13733(void)
{
	(void)NULL;
	return b13733_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_xkbcommon_ok_u_13733(void)
    __attribute__((alias("gj_xkbcommon_ok_u_13733")));
