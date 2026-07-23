/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13126: FriBidi soft-ok unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_fribidi_ok_u_13126(void);
 *     - Always returns 0. Soft stub for fontconfig/freetype soft-id
 *       continuum; FriBidi is intentionally not reported ok.
 *   uint32_t __gj_fribidi_ok_u_13126  (alias)
 *   __libcgj_batch13126_marker = "libcgj-batch13126"
 *
 * Exclusive continuum CREATE-ONLY (13121-13130: fontconfig/freetype
 * soft id stubs — fontconfig_ok_u_13121, freetype_ok_u_13122,
 * harfbuzz_ok_u_13123, cairo_ok_u_13124, pango_ok_u_13125,
 * fribidi_ok_u_13126, graphite_ok_u_13127, font_match_ok_u_13128,
 * font_soft_ready_u_13129→1, batch_id_13130→13130; others →0).
 * Unique surface only; no multi-def. Distinct from
 * gj_fribidi_ok_u_12926 (batch12926), gj_fribidi_ok_u_12726
 * (batch12726), gj_fribidi_ok_u_12526 (batch12526),
 * gj_fribidi_ok_u_12326 (batch12326) and gj_fribidi_ok_u_12126
 * (batch12126). No parent wires. No __int128. No FriBidi
 * implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13126_marker[] = "libcgj-batch13126";

/* Soft FriBidi-ok lamp: always off for this continuum. */
#define B13126_FRIBIDI_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13126_fribidi_ok(void)
{
	return B13126_FRIBIDI_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fribidi_ok_u_13126 - FriBidi soft-ok unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not run bidi algorithms
 * or call FriBidi/libc. No parent wires.
 */
uint32_t
gj_fribidi_ok_u_13126(void)
{
	(void)NULL;
	return b13126_fribidi_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fribidi_ok_u_13126(void)
    __attribute__((alias("gj_fribidi_ok_u_13126")));
