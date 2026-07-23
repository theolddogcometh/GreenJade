/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11027: Graphite soft-ok unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_graphite_ok_u_11027(void);
 *     - Always returns 0. Soft stub for fontconfig/freetype soft-id
 *       continuum; Graphite is intentionally not reported ok.
 *   uint32_t __gj_graphite_ok_u_11027  (alias)
 *   __libcgj_batch11027_marker = "libcgj-batch11027"
 *
 * Exclusive continuum CREATE-ONLY (11021-11030: fontconfig/freetype
 * soft id stubs — fontconfig_ok_u_11021, freetype_ok_u_11022,
 * harfbuzz_ok_u_11023, cairo_ok_u_11024, pango_ok_u_11025,
 * fribidi_ok_u_11026, graphite_ok_u_11027, font_match_ok_u_11028,
 * font_soft_ready_u_11029, batch_id_11030).
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

const char __libcgj_batch11027_marker[] = "libcgj-batch11027";

/* Soft Graphite-ok lamp: always off for this continuum. */
#define B11027_GRAPHITE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11027_graphite_ok(void)
{
	return B11027_GRAPHITE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_graphite_ok_u_11027 - Graphite soft-ok unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not load Graphite faces
 * or call Graphite/libc. No parent wires.
 */
uint32_t
gj_graphite_ok_u_11027(void)
{
	(void)NULL;
	return b11027_graphite_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_graphite_ok_u_11027(void)
    __attribute__((alias("gj_graphite_ok_u_11027")));
