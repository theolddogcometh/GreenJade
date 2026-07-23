/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13327: Graphite soft-ok unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_graphite_ok_u_13327(void);
 *     - Always returns 0. Soft stub for fontconfig/freetype soft-id
 *       continuum; Graphite is intentionally not reported ok.
 *   uint32_t __gj_graphite_ok_u_13327  (alias)
 *   __libcgj_batch13327_marker = "libcgj-batch13327"
 *
 * Exclusive continuum CREATE-ONLY (13321-13330: fontconfig/freetype
 * soft id stubs — fontconfig_ok_u_13321, freetype_ok_u_13322,
 * harfbuzz_ok_u_13323, cairo_ok_u_13324, pango_ok_u_13325,
 * fribidi_ok_u_13326, graphite_ok_u_13327, font_match_ok_u_13328,
 * font_soft_ready_u_13329, batch_id_13330).
 * Unique surface only; no multi-def. Distinct from
 * gj_graphite_ok_u_10127 (batch10127). No parent wires. No __int128.
 * No Graphite implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13327_marker[] = "libcgj-batch13327";

/* Soft Graphite-ok lamp: always off for this continuum. */
#define B13327_GRAPHITE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13327_graphite_ok(void)
{
	return B13327_GRAPHITE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_graphite_ok_u_13327 - Graphite soft-ok unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not load Graphite faces
 * or call Graphite/libc. No parent wires.
 */
uint32_t
gj_graphite_ok_u_13327(void)
{
	(void)NULL;
	return b13327_graphite_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_graphite_ok_u_13327(void)
    __attribute__((alias("gj_graphite_ok_u_13327")));
