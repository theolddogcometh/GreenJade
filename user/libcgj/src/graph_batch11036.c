/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11036: locale IM soft identity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_locale_im_ok_u_11036(void);
 *     - Returns 0 (soft: locale input-method path not ready at compile
 *       time).
 *   uint32_t __gj_locale_im_ok_u_11036  (alias)
 *   __libcgj_batch11036_marker = "libcgj-batch11036"
 *
 * Exclusive CREATE-ONLY wave 11031-11040 (ibus/fcitx soft id stubs).
 * Unique gj_locale_im_ok_u_11036 surface only; no multi-def. No parent
 * wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11036_marker[] = "libcgj-batch11036";

/* Soft locale IM ok lamp: not ready at compile time. */
#define B11036_LOCALE_IM_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11036_ok(void)
{
	return B11036_LOCALE_IM_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_locale_im_ok_u_11036 - soft locale input-method path ok lamp.
 *
 * Always returns 0 (not ready at compile time). Does not query locale
 * IM settings or call libc. No parent wires.
 */
uint32_t
gj_locale_im_ok_u_11036(void)
{
	(void)NULL;
	return b11036_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_locale_im_ok_u_11036(void)
    __attribute__((alias("gj_locale_im_ok_u_11036")));
