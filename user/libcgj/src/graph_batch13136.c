/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13136: IM soft locale-im ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_locale_im_ok_u_13136(void);
 *     - Returns 0 (soft stub: locale-driven input-method selection
 *       slot is soft/unprobed; not a runtime LC_CTYPE / LANG probe).
 *   uint32_t __gj_locale_im_ok_u_13136  (alias)
 *   __libcgj_batch13136_marker = "libcgj-batch13136"
 *
 * Exclusive continuum CREATE-ONLY (13131-13140: IM soft stubs —
 * ok slots all→0; im_soft_ready→1; batch_id→13140 —
 * ibus_ok_u_13131, fcitx_ok_u_13132, xkbcommon_ok_u_13133,
 * compose_ok_u_13134, im_module_ok_u_13135, locale_im_ok_u_13136,
 * input_method_ok_u_13137, keyboard_layout_ok_u_13138,
 * im_soft_ready_u_13139, batch_id_13140). Unique
 * gj_locale_im_ok_u_13136 surface only; no multi-def. Distinct from
 * sibling IM soft ok_u stubs in this wave and prior
 * gj_locale_im_ok_u_12936 (batch12936) / gj_locale_im_ok_u_10436
 * (batch10436). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13136_marker[] = "libcgj-batch13136";

/* Soft-stub locale-im ok value (unprobed). */
#define B13136_LOCALE_IM_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13136_locale_im_ok(void)
{
	return B13136_LOCALE_IM_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_locale_im_ok_u_13136 - soft locale-im ok stub.
 *
 * Always returns 0 (soft/unprobed locale-im slot). Does not call libc
 * or read LC_CTYPE/LANG. No parent wires.
 */
uint32_t
gj_locale_im_ok_u_13136(void)
{
	(void)NULL;
	return b13136_locale_im_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_locale_im_ok_u_13136(void)
    __attribute__((alias("gj_locale_im_ok_u_13136")));
