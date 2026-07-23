/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11628: font match soft-ok unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_font_match_ok_u_11628(void);
 *     - Always returns 0. Soft stub for fontconfig/freetype soft-id
 *       continuum; font match is intentionally not reported ok.
 *   uint32_t __gj_font_match_ok_u_11628  (alias)
 *   __libcgj_batch11628_marker = "libcgj-batch11628"
 *
 * Exclusive continuum CREATE-ONLY (11621-11630: fontconfig/freetype
 * soft id stubs — fontconfig_ok_u_11621, freetype_ok_u_11622,
 * harfbuzz_ok_u_11623, cairo_ok_u_11624, pango_ok_u_11625,
 * fribidi_ok_u_11626, graphite_ok_u_11627, font_match_ok_u_11628,
 * font_soft_ready_u_11629, batch_id_11630).
 * Unique surface only; no multi-def. Distinct from
 * gj_font_match_ok_u_10128 (batch10128) and gj_fc_match_ok_u_9048
 * (batch9048). No parent wires. No __int128. No fontconfig match
 * implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11628_marker[] = "libcgj-batch11628";

/* Soft font-match-ok lamp: always off for this continuum. */
#define B11628_FONT_MATCH_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11628_font_match_ok(void)
{
	return B11628_FONT_MATCH_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_font_match_ok_u_11628 - font match soft-ok unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not run FcFontMatch or
 * call fontconfig/libc. No parent wires.
 */
uint32_t
gj_font_match_ok_u_11628(void)
{
	(void)NULL;
	return b11628_font_match_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_font_match_ok_u_11628(void)
    __attribute__((alias("gj_font_match_ok_u_11628")));
