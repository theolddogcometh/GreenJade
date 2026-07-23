/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13729: font soft continuum lamp (on).
 *
 * Surface (unique symbols):
 *   uint32_t gj_font_soft_ready_u_13729(void);
 *     - Returns 1 (soft lamp only). Indicates the fontconfig/freetype
 *       soft-stub surfaces in this continuum are present; not runtime
 *       library readiness.
 *   uint32_t __gj_font_soft_ready_u_13729  (alias)
 *   __libcgj_batch13729_marker = "libcgj-batch13729"
 *
 * Exclusive continuum CREATE-ONLY (13721-13730: fontconfig/freetype
 * soft id stubs — fontconfig_ok_u_13721, freetype_ok_u_13722,
 * harfbuzz_ok_u_13723, cairo_ok_u_13724, pango_ok_u_13725,
 * fribidi_ok_u_13726, graphite_ok_u_13727, font_match_ok_u_13728,
 * font_soft_ready_u_13729→1, batch_id_13730→13730; others →0).
 * Unique surface only; no multi-def. Per-lib ok units remain 0.
 * Distinct from gj_font_soft_ready_u_13529 (batch13529),
 * gj_font_soft_ready_u_13329 (batch13329), gj_font_soft_ready_u_13129
 * (batch13129) and gj_fontconfig_ready_u_9049 (batch9049). No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13729_marker[] = "libcgj-batch13729";

/* Soft continuum lamp: surfaces present (soft ready only). */
#define B13729_FONT_SOFT_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13729_font_soft_ready(void)
{
	return B13729_FONT_SOFT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_font_soft_ready_u_13729 - font soft continuum ready lamp.
 *
 * Always returns 1. Soft pure-data product tag that exclusive wave
 * 13721-13730 surfaces are present. Does not claim runtime fontconfig
 * / FreeType readiness and does not call libc. No parent wires.
 */
uint32_t
gj_font_soft_ready_u_13729(void)
{
	(void)NULL;
	return b13729_font_soft_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_font_soft_ready_u_13729(void)
    __attribute__((alias("gj_font_soft_ready_u_13729")));
