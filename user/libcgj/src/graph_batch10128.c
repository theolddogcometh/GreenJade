/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10128: font match soft-ok unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_font_match_ok_u_10128(void);
 *     - Always returns 0. Soft stub for fontconfig/freetype soft-id
 *       continuum; font match is intentionally not reported ok.
 *   uint32_t __gj_font_match_ok_u_10128  (alias)
 *   __libcgj_batch10128_marker = "libcgj-batch10128"
 *
 * Exclusive continuum CREATE-ONLY (10121-10130: fontconfig/freetype
 * soft id stubs — fontconfig_ok_u_10121, freetype_ok_u_10122,
 * harfbuzz_ok_u_10123, cairo_ok_u_10124, pango_ok_u_10125,
 * fribidi_ok_u_10126, graphite_ok_u_10127, font_match_ok_u_10128,
 * font_soft_ready_u_10129, batch_id_10130).
 * Unique surface only; no multi-def. Distinct from
 * gj_fc_match_ok_u_9048 (batch9048). No parent wires. No __int128.
 * No fontconfig match implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10128_marker[] = "libcgj-batch10128";

/* Soft font-match-ok lamp: always off for this continuum. */
#define B10128_FONT_MATCH_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10128_font_match_ok(void)
{
	return B10128_FONT_MATCH_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_font_match_ok_u_10128 - font match soft-ok unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not run FcFontMatch or
 * call fontconfig/libc. No parent wires.
 */
uint32_t
gj_font_match_ok_u_10128(void)
{
	(void)NULL;
	return b10128_font_match_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_font_match_ok_u_10128(void)
    __attribute__((alias("gj_font_match_ok_u_10128")));
