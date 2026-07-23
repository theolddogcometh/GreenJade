/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12924: Cairo soft-ok unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_cairo_ok_u_12924(void);
 *     - Always returns 0. Soft stub for fontconfig/freetype soft-id
 *       continuum; Cairo is intentionally not reported ok.
 *   uint32_t __gj_cairo_ok_u_12924  (alias)
 *   __libcgj_batch12924_marker = "libcgj-batch12924"
 *
 * Exclusive continuum CREATE-ONLY (12921-12930: fontconfig/freetype
 * soft id stubs — fontconfig_ok_u_12921, freetype_ok_u_12922,
 * harfbuzz_ok_u_12923, cairo_ok_u_12924, pango_ok_u_12925,
 * fribidi_ok_u_12926, graphite_ok_u_12927, font_match_ok_u_12928,
 * font_soft_ready_u_12929→1, batch_id_12930→12930; others →0).
 * Unique surface only; no multi-def. Distinct from
 * gj_cairo_ok_u_12724 (batch12724), gj_cairo_ok_u_12524 (batch12524),
 * gj_cairo_ok_u_12324 (batch12324) and gj_cairo_ok_u_12124
 * (batch12124). No parent wires. No __int128.
 * No Cairo implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12924_marker[] = "libcgj-batch12924";

/* Soft Cairo-ok lamp: always off for this continuum. */
#define B12924_CAIRO_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12924_cairo_ok(void)
{
	return B12924_CAIRO_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cairo_ok_u_12924 - Cairo soft-ok unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not create cairo_t or
 * call Cairo/libc. No parent wires.
 */
uint32_t
gj_cairo_ok_u_12924(void)
{
	(void)NULL;
	return b12924_cairo_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cairo_ok_u_12924(void)
    __attribute__((alias("gj_cairo_ok_u_12924")));
