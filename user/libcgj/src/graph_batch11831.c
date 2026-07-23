/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11831: ibus soft identity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ibus_ok_u_11831(void);
 *     - Returns 0 (soft: ibus input method not ready at compile time).
 *   uint32_t __gj_ibus_ok_u_11831  (alias)
 *   __libcgj_batch11831_marker = "libcgj-batch11831"
 *
 * Exclusive CREATE-ONLY wave 11831-11840 (ibus/fcitx soft id stubs).
 * Unique gj_ibus_ok_u_11831 surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11831_marker[] = "libcgj-batch11831";

/* Soft ibus ok lamp: not ready at compile time. */
#define B11831_IBUS_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11831_ok(void)
{
	return B11831_IBUS_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ibus_ok_u_11831 - soft ibus input-method ok lamp.
 *
 * Always returns 0 (not ready at compile time). Does not probe ibus
 * daemon or call libc. No parent wires.
 */
uint32_t
gj_ibus_ok_u_11831(void)
{
	(void)NULL;
	return b11831_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ibus_ok_u_11831(void)
    __attribute__((alias("gj_ibus_ok_u_11831")));
