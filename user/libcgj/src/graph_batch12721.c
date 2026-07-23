/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12721: fontconfig soft-ok unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_fontconfig_ok_u_12721(void);
 *     - Always returns 0. Soft stub for fontconfig/freetype soft-id
 *       continuum; fontconfig is intentionally not reported ok.
 *   uint32_t __gj_fontconfig_ok_u_12721  (alias)
 *   __libcgj_batch12721_marker = "libcgj-batch12721"
 *
 * Exclusive continuum CREATE-ONLY (12721-12730: fontconfig/freetype
 * soft id stubs — fontconfig_ok_u_12721, freetype_ok_u_12722,
 * harfbuzz_ok_u_12723, cairo_ok_u_12724, pango_ok_u_12725,
 * fribidi_ok_u_12726, graphite_ok_u_12727, font_match_ok_u_12728,
 * font_soft_ready_u_12729→1, batch_id_12730→12730; others →0).
 * Unique surface only; no multi-def. Distinct from
 * gj_fontconfig_ok_u_12521 (batch12521), gj_fontconfig_ok_u_12321
 * (batch12321), gj_fontconfig_ok_u_12121 (batch12121) and
 * gj_fontconfig_ready_u_9049 (batch9049). No parent wires. No __int128.
 * No fontconfig / FreeType implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12721_marker[] = "libcgj-batch12721";

/* Soft fontconfig-ok lamp: always off for this continuum. */
#define B12721_FONTCONFIG_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12721_fontconfig_ok(void)
{
	return B12721_FONTCONFIG_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fontconfig_ok_u_12721 - fontconfig soft-ok unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not open FcConfig or call
 * fontconfig/libc. No parent wires.
 */
uint32_t
gj_fontconfig_ok_u_12721(void)
{
	(void)NULL;
	return b12721_fontconfig_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fontconfig_ok_u_12721(void)
    __attribute__((alias("gj_fontconfig_ok_u_12721")));
