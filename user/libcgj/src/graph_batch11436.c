/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11436: IM soft locale-im ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_locale_im_ok_u_11436(void);
 *     - Returns 0 (soft stub: locale-driven input-method selection
 *       slot is soft/unprobed; not a runtime LC_CTYPE / LANG probe).
 *   uint32_t __gj_locale_im_ok_u_11436  (alias)
 *   __libcgj_batch11436_marker = "libcgj-batch11436"
 *
 * Exclusive continuum CREATE-ONLY (11431-11440: IM soft stubs —
 * ok slots all→0; im_soft_ready→1; batch_id→11440). Unique
 * gj_locale_im_ok_u_11436 surface only; no multi-def. Distinct from
 * sibling IM soft ok_u stubs in this wave and prior gj_locale_im_ok_u_*
 * waves. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11436_marker[] = "libcgj-batch11436";

/* Soft-stub locale-im ok value (unprobed). */
#define B11436_LOCALE_IM_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11436_locale_im_ok(void)
{
	return B11436_LOCALE_IM_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_locale_im_ok_u_11436 - soft locale-im ok stub.
 *
 * Always returns 0 (soft/unprobed locale-im slot). Does not call libc
 * or read LC_CTYPE/LANG. No parent wires.
 */
uint32_t
gj_locale_im_ok_u_11436(void)
{
	(void)NULL;
	return b11436_locale_im_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_locale_im_ok_u_11436(void)
    __attribute__((alias("gj_locale_im_ok_u_11436")));
