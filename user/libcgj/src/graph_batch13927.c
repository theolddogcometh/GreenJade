/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13927: Graphite soft-ok unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_graphite_ok_u_13927(void);
 *     - Always returns 0. Soft stub for fontconfig/freetype soft-id
 *       continuum; Graphite is intentionally not reported ok.
 *   uint32_t __gj_graphite_ok_u_13927  (alias)
 *   __libcgj_batch13927_marker = "libcgj-batch13927"
 *
 * Exclusive continuum CREATE-ONLY (13921-13930: fontconfig/freetype
 * soft id stubs — fontconfig_ok_u_13921, freetype_ok_u_13922,
 * harfbuzz_ok_u_13923, cairo_ok_u_13924, pango_ok_u_13925,
 * fribidi_ok_u_13926, graphite_ok_u_13927, font_match_ok_u_13928,
 * font_soft_ready_u_13929→1, batch_id_13930→13930; others →0).
 * Unique surface only; no multi-def. Distinct from
 * gj_graphite_ok_u_13727 (batch13727), gj_graphite_ok_u_13327
 * (batch13327), gj_graphite_ok_u_13127 (batch13127) and
 * gj_graphite_ok_u_10127 (batch10127). No parent wires. No __int128.
 * No Graphite implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13927_marker[] = "libcgj-batch13927";

/* Soft Graphite-ok lamp: always off for this continuum. */
#define B13927_GRAPHITE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13927_graphite_ok(void)
{
	return B13927_GRAPHITE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_graphite_ok_u_13927 - Graphite soft-ok unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not load Graphite faces
 * or call Graphite/libc. No parent wires.
 */
uint32_t
gj_graphite_ok_u_13927(void)
{
	(void)NULL;
	return b13927_graphite_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_graphite_ok_u_13927(void)
    __attribute__((alias("gj_graphite_ok_u_13927")));
