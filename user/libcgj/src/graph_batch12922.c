/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12922: FreeType soft-ok unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_freetype_ok_u_12922(void);
 *     - Always returns 0. Soft stub for fontconfig/freetype soft-id
 *       continuum; FreeType is intentionally not reported ok.
 *   uint32_t __gj_freetype_ok_u_12922  (alias)
 *   __libcgj_batch12922_marker = "libcgj-batch12922"
 *
 * Exclusive continuum CREATE-ONLY (12921-12930: fontconfig/freetype
 * soft id stubs — fontconfig_ok_u_12921, freetype_ok_u_12922,
 * harfbuzz_ok_u_12923, cairo_ok_u_12924, pango_ok_u_12925,
 * fribidi_ok_u_12926, graphite_ok_u_12927, font_match_ok_u_12928,
 * font_soft_ready_u_12929→1, batch_id_12930→12930; others →0).
 * Unique surface only; no multi-def. Distinct from
 * gj_freetype_ok_u_12722 (batch12722), gj_freetype_ok_u_12522
 * (batch12522), gj_freetype_ok_u_12322 (batch12322) and
 * gj_freetype_ok_u_12122 (batch12122). No parent wires. No __int128.
 * No FreeType implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12922_marker[] = "libcgj-batch12922";

/* Soft FreeType-ok lamp: always off for this continuum. */
#define B12922_FREETYPE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12922_freetype_ok(void)
{
	return B12922_FREETYPE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_freetype_ok_u_12922 - FreeType soft-ok unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not open FT_Library or
 * call FreeType/libc. No parent wires.
 */
uint32_t
gj_freetype_ok_u_12922(void)
{
	(void)NULL;
	return b12922_freetype_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_freetype_ok_u_12922(void)
    __attribute__((alias("gj_freetype_ok_u_12922")));
