/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11029: font soft continuum lamp (on).
 *
 * Surface (unique symbols):
 *   uint32_t gj_font_soft_ready_u_11029(void);
 *     - Returns 1 (soft lamp only). Indicates the fontconfig/freetype
 *       soft-stub surfaces in this continuum are present; not runtime
 *       library readiness.
 *   uint32_t __gj_font_soft_ready_u_11029  (alias)
 *   __libcgj_batch11029_marker = "libcgj-batch11029"
 *
 * Exclusive continuum CREATE-ONLY (11021-11030: fontconfig/freetype
 * soft id stubs — fontconfig_ok_u_11021, freetype_ok_u_11022,
 * harfbuzz_ok_u_11023, cairo_ok_u_11024, pango_ok_u_11025,
 * fribidi_ok_u_11026, graphite_ok_u_11027, font_match_ok_u_11028,
 * font_soft_ready_u_11029, batch_id_11030).
 * Unique surface only; no multi-def. Per-lib ok units remain 0.
 * Distinct from gj_font_soft_ready_u_10129 (batch10129). No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11029_marker[] = "libcgj-batch11029";

/* Soft continuum lamp: surfaces present (soft ready only). */
#define B11029_FONT_SOFT_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11029_font_soft_ready(void)
{
	return B11029_FONT_SOFT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_font_soft_ready_u_11029 - font soft continuum ready lamp.
 *
 * Always returns 1. Soft pure-data product tag that exclusive wave
 * 11021-11030 surfaces are present. Does not claim runtime fontconfig
 * / FreeType readiness and does not call libc. No parent wires.
 */
uint32_t
gj_font_soft_ready_u_11029(void)
{
	(void)NULL;
	return b11029_font_soft_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_font_soft_ready_u_11029(void)
    __attribute__((alias("gj_font_soft_ready_u_11029")));
