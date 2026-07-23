/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12722: FreeType soft-ok unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_freetype_ok_u_12722(void);
 *     - Always returns 0. Soft stub for fontconfig/freetype soft-id
 *       continuum; FreeType is intentionally not reported ok.
 *   uint32_t __gj_freetype_ok_u_12722  (alias)
 *   __libcgj_batch12722_marker = "libcgj-batch12722"
 *
 * Exclusive continuum CREATE-ONLY (12721-12730: fontconfig/freetype
 * soft id stubs — fontconfig_ok_u_12721, freetype_ok_u_12722,
 * harfbuzz_ok_u_12723, cairo_ok_u_12724, pango_ok_u_12725,
 * fribidi_ok_u_12726, graphite_ok_u_12727, font_match_ok_u_12728,
 * font_soft_ready_u_12729→1, batch_id_12730→12730; others →0).
 * Unique surface only; no multi-def. Distinct from
 * gj_freetype_ok_u_12522 (batch12522), gj_freetype_ok_u_12322
 * (batch12322) and gj_freetype_ok_u_12122 (batch12122). No parent
 * wires. No __int128. No FreeType implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12722_marker[] = "libcgj-batch12722";

/* Soft FreeType-ok lamp: always off for this continuum. */
#define B12722_FREETYPE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12722_freetype_ok(void)
{
	return B12722_FREETYPE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_freetype_ok_u_12722 - FreeType soft-ok unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not open FT_Library or
 * call FreeType/libc. No parent wires.
 */
uint32_t
gj_freetype_ok_u_12722(void)
{
	(void)NULL;
	return b12722_freetype_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_freetype_ok_u_12722(void)
    __attribute__((alias("gj_freetype_ok_u_12722")));
