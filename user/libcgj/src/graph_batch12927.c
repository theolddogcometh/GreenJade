/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12927: Graphite soft-ok unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_graphite_ok_u_12927(void);
 *     - Always returns 0. Soft stub for fontconfig/freetype soft-id
 *       continuum; Graphite is intentionally not reported ok.
 *   uint32_t __gj_graphite_ok_u_12927  (alias)
 *   __libcgj_batch12927_marker = "libcgj-batch12927"
 *
 * Exclusive continuum CREATE-ONLY (12921-12930: fontconfig/freetype
 * soft id stubs — fontconfig_ok_u_12921, freetype_ok_u_12922,
 * harfbuzz_ok_u_12923, cairo_ok_u_12924, pango_ok_u_12925,
 * fribidi_ok_u_12926, graphite_ok_u_12927, font_match_ok_u_12928,
 * font_soft_ready_u_12929→1, batch_id_12930→12930; others →0).
 * Unique surface only; no multi-def. Distinct from
 * gj_graphite_ok_u_12727 (batch12727), gj_graphite_ok_u_12527
 * (batch12527), gj_graphite_ok_u_12327 (batch12327) and
 * gj_graphite_ok_u_12127 (batch12127). No parent wires. No __int128.
 * No Graphite implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12927_marker[] = "libcgj-batch12927";

/* Soft Graphite-ok lamp: always off for this continuum. */
#define B12927_GRAPHITE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12927_graphite_ok(void)
{
	return B12927_GRAPHITE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_graphite_ok_u_12927 - Graphite soft-ok unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not load Graphite faces
 * or call Graphite/libc. No parent wires.
 */
uint32_t
gj_graphite_ok_u_12927(void)
{
	(void)NULL;
	return b12927_graphite_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_graphite_ok_u_12927(void)
    __attribute__((alias("gj_graphite_ok_u_12927")));
