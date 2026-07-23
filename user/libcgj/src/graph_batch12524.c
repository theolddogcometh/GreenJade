/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12524: Cairo soft-ok unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_cairo_ok_u_12524(void);
 *     - Always returns 0. Soft stub for fontconfig/freetype soft-id
 *       continuum; Cairo is intentionally not reported ok.
 *   uint32_t __gj_cairo_ok_u_12524  (alias)
 *   __libcgj_batch12524_marker = "libcgj-batch12524"
 *
 * Exclusive continuum CREATE-ONLY (12521-12530: fontconfig/freetype
 * soft id stubs — fontconfig_ok_u_12521, freetype_ok_u_12522,
 * harfbuzz_ok_u_12523, cairo_ok_u_12524, pango_ok_u_12525,
 * fribidi_ok_u_12526, graphite_ok_u_12527, font_match_ok_u_12528,
 * font_soft_ready_u_12529, batch_id_12530).
 * Unique surface only; no multi-def. Distinct from
 * gj_cairo_ok_u_12324 (batch12324). No parent wires. No __int128.
 * No Cairo implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12524_marker[] = "libcgj-batch12524";

/* Soft Cairo-ok lamp: always off for this continuum. */
#define B12524_CAIRO_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12524_cairo_ok(void)
{
	return B12524_CAIRO_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cairo_ok_u_12524 - Cairo soft-ok unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not create cairo_t or
 * call Cairo/libc. No parent wires.
 */
uint32_t
gj_cairo_ok_u_12524(void)
{
	(void)NULL;
	return b12524_cairo_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cairo_ok_u_12524(void)
    __attribute__((alias("gj_cairo_ok_u_12524")));
