/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13322: FreeType soft-ok unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_freetype_ok_u_13322(void);
 *     - Always returns 0. Soft stub for fontconfig/freetype soft-id
 *       continuum; FreeType is intentionally not reported ok.
 *   uint32_t __gj_freetype_ok_u_13322  (alias)
 *   __libcgj_batch13322_marker = "libcgj-batch13322"
 *
 * Exclusive continuum CREATE-ONLY (13321-13330: fontconfig/freetype
 * soft id stubs — fontconfig_ok_u_13321, freetype_ok_u_13322,
 * harfbuzz_ok_u_13323, cairo_ok_u_13324, pango_ok_u_13325,
 * fribidi_ok_u_13326, graphite_ok_u_13327, font_match_ok_u_13328,
 * font_soft_ready_u_13329, batch_id_13330).
 * Unique surface only; no multi-def. Distinct from
 * gj_freetype_ok_u_10122 (batch10122). No parent wires. No __int128.
 * No FreeType implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13322_marker[] = "libcgj-batch13322";

/* Soft FreeType-ok lamp: always off for this continuum. */
#define B13322_FREETYPE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13322_freetype_ok(void)
{
	return B13322_FREETYPE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_freetype_ok_u_13322 - FreeType soft-ok unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not open FT_Library or
 * call FreeType/libc. No parent wires.
 */
uint32_t
gj_freetype_ok_u_13322(void)
{
	(void)NULL;
	return b13322_freetype_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_freetype_ok_u_13322(void)
    __attribute__((alias("gj_freetype_ok_u_13322")));
