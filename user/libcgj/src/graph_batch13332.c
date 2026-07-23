/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13332: fcitx soft identity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fcitx_ok_u_13332(void);
 *     - Returns 0 (soft: fcitx input method not ready at compile time).
 *   uint32_t __gj_fcitx_ok_u_13332  (alias)
 *   __libcgj_batch13332_marker = "libcgj-batch13332"
 *
 * Exclusive CREATE-ONLY wave 13331-13340 (ibus/fcitx soft id stubs).
 * Unique gj_fcitx_ok_u_13332 surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13332_marker[] = "libcgj-batch13332";

/* Soft fcitx ok lamp: not ready at compile time. */
#define B13332_FCITX_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13332_ok(void)
{
	return B13332_FCITX_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fcitx_ok_u_13332 - soft fcitx input-method ok lamp.
 *
 * Always returns 0 (not ready at compile time). Does not probe fcitx
 * daemon or call libc. No parent wires.
 */
uint32_t
gj_fcitx_ok_u_13332(void)
{
	(void)NULL;
	return b13332_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fcitx_ok_u_13332(void)
    __attribute__((alias("gj_fcitx_ok_u_13332")));
