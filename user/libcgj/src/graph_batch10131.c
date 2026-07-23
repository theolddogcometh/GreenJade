/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10131: ibus soft identity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ibus_ok_u_10131(void);
 *     - Returns 0 (soft: ibus input method not ready at compile time).
 *   uint32_t __gj_ibus_ok_u_10131  (alias)
 *   __libcgj_batch10131_marker = "libcgj-batch10131"
 *
 * Exclusive CREATE-ONLY wave 10131-10140 (ibus/fcitx soft id stubs).
 * Unique gj_ibus_ok_u_10131 surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10131_marker[] = "libcgj-batch10131";

/* Soft ibus ok lamp: not ready at compile time. */
#define B10131_IBUS_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10131_ok(void)
{
	return B10131_IBUS_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ibus_ok_u_10131 - soft ibus input-method ok lamp.
 *
 * Always returns 0 (not ready at compile time). Does not probe ibus
 * daemon or call libc. No parent wires.
 */
uint32_t
gj_ibus_ok_u_10131(void)
{
	(void)NULL;
	return b10131_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ibus_ok_u_10131(void)
    __attribute__((alias("gj_ibus_ok_u_10131")));
