/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13725: Pango soft-ok unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pango_ok_u_13725(void);
 *     - Always returns 0. Soft stub for fontconfig/freetype soft-id
 *       continuum; Pango is intentionally not reported ok.
 *   uint32_t __gj_pango_ok_u_13725  (alias)
 *   __libcgj_batch13725_marker = "libcgj-batch13725"
 *
 * Exclusive continuum CREATE-ONLY (13721-13730: fontconfig/freetype
 * soft id stubs — fontconfig_ok_u_13721, freetype_ok_u_13722,
 * harfbuzz_ok_u_13723, cairo_ok_u_13724, pango_ok_u_13725,
 * fribidi_ok_u_13726, graphite_ok_u_13727, font_match_ok_u_13728,
 * font_soft_ready_u_13729→1, batch_id_13730→13730; others →0).
 * Unique surface only; no multi-def. Distinct from
 * gj_pango_ok_u_13524 (batch13524), gj_pango_ok_u_13325 (batch13325)
 * and gj_pango_ok_u_13125 (batch13125). No parent wires. No __int128.
 * No Pango implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13725_marker[] = "libcgj-batch13725";

/* Soft Pango-ok lamp: always off for this continuum. */
#define B13725_PANGO_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13725_pango_ok(void)
{
	return B13725_PANGO_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pango_ok_u_13725 - Pango soft-ok unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not create layouts or
 * call Pango/libc. No parent wires.
 */
uint32_t
gj_pango_ok_u_13725(void)
{
	(void)NULL;
	return b13725_pango_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pango_ok_u_13725(void)
    __attribute__((alias("gj_pango_ok_u_13725")));
