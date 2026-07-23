/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13127: Graphite soft-ok unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_graphite_ok_u_13127(void);
 *     - Always returns 0. Soft stub for fontconfig/freetype soft-id
 *       continuum; Graphite is intentionally not reported ok.
 *   uint32_t __gj_graphite_ok_u_13127  (alias)
 *   __libcgj_batch13127_marker = "libcgj-batch13127"
 *
 * Exclusive continuum CREATE-ONLY (13121-13130: fontconfig/freetype
 * soft id stubs — fontconfig_ok_u_13121, freetype_ok_u_13122,
 * harfbuzz_ok_u_13123, cairo_ok_u_13124, pango_ok_u_13125,
 * fribidi_ok_u_13126, graphite_ok_u_13127, font_match_ok_u_13128,
 * font_soft_ready_u_13129→1, batch_id_13130→13130; others →0).
 * Unique surface only; no multi-def. Distinct from
 * gj_graphite_ok_u_12927 (batch12927), gj_graphite_ok_u_12727
 * (batch12727), gj_graphite_ok_u_12527 (batch12527),
 * gj_graphite_ok_u_12327 (batch12327) and gj_graphite_ok_u_12127
 * (batch12127). No parent wires. No __int128. No Graphite
 * implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13127_marker[] = "libcgj-batch13127";

/* Soft Graphite-ok lamp: always off for this continuum. */
#define B13127_GRAPHITE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13127_graphite_ok(void)
{
	return B13127_GRAPHITE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_graphite_ok_u_13127 - Graphite soft-ok unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not load Graphite faces
 * or call Graphite/libc. No parent wires.
 */
uint32_t
gj_graphite_ok_u_13127(void)
{
	(void)NULL;
	return b13127_graphite_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_graphite_ok_u_13127(void)
    __attribute__((alias("gj_graphite_ok_u_13127")));
