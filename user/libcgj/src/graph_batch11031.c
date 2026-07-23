/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11031: ibus soft identity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ibus_ok_u_11031(void);
 *     - Returns 0 (soft: ibus input method not ready at compile time).
 *   uint32_t __gj_ibus_ok_u_11031  (alias)
 *   __libcgj_batch11031_marker = "libcgj-batch11031"
 *
 * Exclusive CREATE-ONLY wave 11031-11040 (ibus/fcitx soft id stubs).
 * Unique gj_ibus_ok_u_11031 surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11031_marker[] = "libcgj-batch11031";

/* Soft ibus ok lamp: not ready at compile time. */
#define B11031_IBUS_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11031_ok(void)
{
	return B11031_IBUS_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ibus_ok_u_11031 - soft ibus input-method ok lamp.
 *
 * Always returns 0 (not ready at compile time). Does not probe ibus
 * daemon or call libc. No parent wires.
 */
uint32_t
gj_ibus_ok_u_11031(void)
{
	(void)NULL;
	return b11031_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ibus_ok_u_11031(void)
    __attribute__((alias("gj_ibus_ok_u_11031")));
