/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11621: fontconfig soft-ok unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_fontconfig_ok_u_11621(void);
 *     - Always returns 0. Soft stub for fontconfig/freetype soft-id
 *       continuum; fontconfig is intentionally not reported ok.
 *   uint32_t __gj_fontconfig_ok_u_11621  (alias)
 *   __libcgj_batch11621_marker = "libcgj-batch11621"
 *
 * Exclusive continuum CREATE-ONLY (11621-11630: fontconfig/freetype
 * soft id stubs — fontconfig_ok_u_11621, freetype_ok_u_11622,
 * harfbuzz_ok_u_11623, cairo_ok_u_11624, pango_ok_u_11625,
 * fribidi_ok_u_11626, graphite_ok_u_11627, font_match_ok_u_11628,
 * font_soft_ready_u_11629, batch_id_11630).
 * Unique surface only; no multi-def. Distinct from
 * gj_fontconfig_ok_u_10121 (batch10121) and gj_fontconfig_ready_u_9049
 * (batch9049). No parent wires. No __int128. No fontconfig / FreeType
 * implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11621_marker[] = "libcgj-batch11621";

/* Soft fontconfig-ok lamp: always off for this continuum. */
#define B11621_FONTCONFIG_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11621_fontconfig_ok(void)
{
	return B11621_FONTCONFIG_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fontconfig_ok_u_11621 - fontconfig soft-ok unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not open FcConfig or call
 * fontconfig/libc. No parent wires.
 */
uint32_t
gj_fontconfig_ok_u_11621(void)
{
	(void)NULL;
	return b11621_fontconfig_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fontconfig_ok_u_11621(void)
    __attribute__((alias("gj_fontconfig_ok_u_11621")));
