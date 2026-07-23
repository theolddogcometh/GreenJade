/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12532: fcitx soft identity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fcitx_ok_u_12532(void);
 *     - Returns 0 (soft: fcitx input method not ready at compile time).
 *   uint32_t __gj_fcitx_ok_u_12532  (alias)
 *   __libcgj_batch12532_marker = "libcgj-batch12532"
 *
 * Exclusive CREATE-ONLY wave 12531-12540 (ibus/fcitx soft id stubs).
 * Unique gj_fcitx_ok_u_12532 surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12532_marker[] = "libcgj-batch12532";

/* Soft fcitx ok lamp: not ready at compile time. */
#define B12532_FCITX_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12532_ok(void)
{
	return B12532_FCITX_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fcitx_ok_u_12532 - soft fcitx input-method ok lamp.
 *
 * Always returns 0 (not ready at compile time). Does not probe fcitx
 * daemon or call libc. No parent wires.
 */
uint32_t
gj_fcitx_ok_u_12532(void)
{
	(void)NULL;
	return b12532_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fcitx_ok_u_12532(void)
    __attribute__((alias("gj_fcitx_ok_u_12532")));
