/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12136: locale IM soft identity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_locale_im_ok_u_12136(void);
 *     - Returns 0 (soft: locale input-method path not ready at compile
 *       time).
 *   uint32_t __gj_locale_im_ok_u_12136  (alias)
 *   __libcgj_batch12136_marker = "libcgj-batch12136"
 *
 * Exclusive CREATE-ONLY wave 12131-12140 (IM soft: ibus/fcitx soft id
 * stubs — ready→1 others 0). Unique gj_locale_im_ok_u_12136 surface
 * only; no multi-def. Distinct from gj_locale_im_ok_u_11836
 * (batch11836), gj_locale_im_ok_u_11636 (batch11636), and sibling IM
 * soft stubs in this wave. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12136_marker[] = "libcgj-batch12136";

/* Soft locale IM ok lamp: not ready at compile time. */
#define B12136_LOCALE_IM_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12136_ok(void)
{
	return B12136_LOCALE_IM_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_locale_im_ok_u_12136 - soft locale input-method ok lamp.
 *
 * Always returns 0 (not ready at compile time). Does not read locale
 * or call libc. No parent wires.
 */
uint32_t
gj_locale_im_ok_u_12136(void)
{
	(void)NULL;
	return b12136_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_locale_im_ok_u_12136(void)
    __attribute__((alias("gj_locale_im_ok_u_12136")));
