/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12121: fontconfig soft-ok unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_fontconfig_ok_u_12121(void);
 *     - Always returns 0. Soft stub for fontconfig/freetype soft-id
 *       continuum; fontconfig is intentionally not reported ok.
 *   uint32_t __gj_fontconfig_ok_u_12121  (alias)
 *   __libcgj_batch12121_marker = "libcgj-batch12121"
 *
 * Exclusive continuum CREATE-ONLY (12121-12130: fontconfig/freetype
 * soft id stubs — fontconfig_ok_u_12121, freetype_ok_u_12122,
 * harfbuzz_ok_u_12123, cairo_ok_u_12124, pango_ok_u_12125,
 * fribidi_ok_u_12126, graphite_ok_u_12127, font_match_ok_u_12128,
 * font_soft_ready_u_12129, batch_id_12130).
 * Unique surface only; no multi-def. Distinct from
 * gj_fontconfig_ok_u_10121 (batch10121) and gj_fontconfig_ready_u_9049
 * (batch9049). No parent wires. No __int128.
 * No fontconfig / FreeType implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12121_marker[] = "libcgj-batch12121";

/* Soft fontconfig-ok lamp: always off for this continuum. */
#define B12121_FONTCONFIG_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12121_fontconfig_ok(void)
{
	return B12121_FONTCONFIG_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fontconfig_ok_u_12121 - fontconfig soft-ok unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not open FcConfig or call
 * fontconfig/libc. No parent wires.
 */
uint32_t
gj_fontconfig_ok_u_12121(void)
{
	(void)NULL;
	return b12121_fontconfig_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fontconfig_ok_u_12121(void)
    __attribute__((alias("gj_fontconfig_ok_u_12121")));
