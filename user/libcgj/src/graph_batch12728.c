/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12728: font match soft-ok unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_font_match_ok_u_12728(void);
 *     - Always returns 0. Soft stub for fontconfig/freetype soft-id
 *       continuum; font match is intentionally not reported ok.
 *   uint32_t __gj_font_match_ok_u_12728  (alias)
 *   __libcgj_batch12728_marker = "libcgj-batch12728"
 *
 * Exclusive continuum CREATE-ONLY (12721-12730: fontconfig/freetype
 * soft id stubs — fontconfig_ok_u_12721, freetype_ok_u_12722,
 * harfbuzz_ok_u_12723, cairo_ok_u_12724, pango_ok_u_12725,
 * fribidi_ok_u_12726, graphite_ok_u_12727, font_match_ok_u_12728,
 * font_soft_ready_u_12729→1, batch_id_12730→12730; others →0).
 * Unique surface only; no multi-def. Distinct from
 * gj_font_match_ok_u_12528 (batch12528), gj_font_match_ok_u_12328
 * (batch12328), gj_font_match_ok_u_12128 (batch12128) and
 * gj_fc_match_ok_u_9048 (batch9048). No parent wires. No __int128.
 * No fontconfig match implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12728_marker[] = "libcgj-batch12728";

/* Soft font-match-ok lamp: always off for this continuum. */
#define B12728_FONT_MATCH_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12728_font_match_ok(void)
{
	return B12728_FONT_MATCH_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_font_match_ok_u_12728 - font match soft-ok unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not run FcFontMatch or
 * call fontconfig/libc. No parent wires.
 */
uint32_t
gj_font_match_ok_u_12728(void)
{
	(void)NULL;
	return b12728_font_match_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_font_match_ok_u_12728(void)
    __attribute__((alias("gj_font_match_ok_u_12728")));
