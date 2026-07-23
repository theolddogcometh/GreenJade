/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13936: locale IM soft identity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_locale_im_ok_u_13936(void);
 *     - Returns 0 (soft: locale input-method path not ready at compile
 *       time).
 *   uint32_t __gj_locale_im_ok_u_13936  (alias)
 *   __libcgj_batch13936_marker = "libcgj-batch13936"
 *
 * Exclusive CREATE-ONLY wave 13931-13940 (ibus/fcitx soft id stubs).
 * Unique gj_locale_im_ok_u_13936 surface only; no multi-def. No parent
 * wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13936_marker[] = "libcgj-batch13936";

/* Soft locale IM ok lamp: not ready at compile time. */
#define B13936_LOCALE_IM_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13936_ok(void)
{
	return B13936_LOCALE_IM_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_locale_im_ok_u_13936 - soft locale input-method ok lamp.
 *
 * Always returns 0 (not ready at compile time). Does not read locale
 * or call libc. No parent wires.
 */
uint32_t
gj_locale_im_ok_u_13936(void)
{
	(void)NULL;
	return b13936_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_locale_im_ok_u_13936(void)
    __attribute__((alias("gj_locale_im_ok_u_13936")));
