/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12326: FriBidi soft-ok unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_fribidi_ok_u_12326(void);
 *     - Always returns 0. Soft stub for fontconfig/freetype soft-id
 *       continuum; FriBidi is intentionally not reported ok.
 *   uint32_t __gj_fribidi_ok_u_12326  (alias)
 *   __libcgj_batch12326_marker = "libcgj-batch12326"
 *
 * Exclusive continuum CREATE-ONLY (12321-12330: fontconfig/freetype
 * soft id stubs — fontconfig_ok_u_12321, freetype_ok_u_12322,
 * harfbuzz_ok_u_12323, cairo_ok_u_12324, pango_ok_u_12325,
 * fribidi_ok_u_12326, graphite_ok_u_12327, font_match_ok_u_12328,
 * font_soft_ready_u_12329, batch_id_12330).
 * Unique surface only; no multi-def. Distinct from
 * gj_fribidi_ok_u_12126 (batch12126). No parent wires. No __int128.
 * No FriBidi implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12326_marker[] = "libcgj-batch12326";

/* Soft FriBidi-ok lamp: always off for this continuum. */
#define B12326_FRIBIDI_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12326_fribidi_ok(void)
{
	return B12326_FRIBIDI_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fribidi_ok_u_12326 - FriBidi soft-ok unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not run bidi algorithms
 * or call FriBidi/libc. No parent wires.
 */
uint32_t
gj_fribidi_ok_u_12326(void)
{
	(void)NULL;
	return b12326_fribidi_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fribidi_ok_u_12326(void)
    __attribute__((alias("gj_fribidi_ok_u_12326")));
