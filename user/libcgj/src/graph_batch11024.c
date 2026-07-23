/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11024: Cairo soft-ok unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_cairo_ok_u_11024(void);
 *     - Always returns 0. Soft stub for fontconfig/freetype soft-id
 *       continuum; Cairo is intentionally not reported ok.
 *   uint32_t __gj_cairo_ok_u_11024  (alias)
 *   __libcgj_batch11024_marker = "libcgj-batch11024"
 *
 * Exclusive continuum CREATE-ONLY (11021-11030: fontconfig/freetype
 * soft id stubs — fontconfig_ok_u_11021, freetype_ok_u_11022,
 * harfbuzz_ok_u_11023, cairo_ok_u_11024, pango_ok_u_11025,
 * fribidi_ok_u_11026, graphite_ok_u_11027, font_match_ok_u_11028,
 * font_soft_ready_u_11029, batch_id_11030).
 * Unique surface only; no multi-def. Distinct from
 * gj_cairo_ok_u_10124 (batch10124). No parent wires. No __int128.
 * No Cairo implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11024_marker[] = "libcgj-batch11024";

/* Soft Cairo-ok lamp: always off for this continuum. */
#define B11024_CAIRO_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11024_cairo_ok(void)
{
	return B11024_CAIRO_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cairo_ok_u_11024 - Cairo soft-ok unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not create cairo_t or
 * call Cairo/libc. No parent wires.
 */
uint32_t
gj_cairo_ok_u_11024(void)
{
	(void)NULL;
	return b11024_cairo_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cairo_ok_u_11024(void)
    __attribute__((alias("gj_cairo_ok_u_11024")));
