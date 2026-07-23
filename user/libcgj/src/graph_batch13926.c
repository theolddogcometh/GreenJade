/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13926: FriBidi soft-ok unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_fribidi_ok_u_13926(void);
 *     - Always returns 0. Soft stub for fontconfig/freetype soft-id
 *       continuum; FriBidi is intentionally not reported ok.
 *   uint32_t __gj_fribidi_ok_u_13926  (alias)
 *   __libcgj_batch13926_marker = "libcgj-batch13926"
 *
 * Exclusive continuum CREATE-ONLY (13921-13930: fontconfig/freetype
 * soft id stubs — fontconfig_ok_u_13921, freetype_ok_u_13922,
 * harfbuzz_ok_u_13923, cairo_ok_u_13924, pango_ok_u_13925,
 * fribidi_ok_u_13926, graphite_ok_u_13927, font_match_ok_u_13928,
 * font_soft_ready_u_13929→1, batch_id_13930→13930; others →0).
 * Unique surface only; no multi-def. Distinct from
 * gj_fribidi_ok_u_13726 (batch13726), gj_fribidi_ok_u_13326
 * (batch13326), gj_fribidi_ok_u_13126 (batch13126) and
 * gj_fribidi_ok_u_10126 (batch10126). No parent wires. No __int128.
 * No FriBidi implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13926_marker[] = "libcgj-batch13926";

/* Soft FriBidi-ok lamp: always off for this continuum. */
#define B13926_FRIBIDI_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13926_fribidi_ok(void)
{
	return B13926_FRIBIDI_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fribidi_ok_u_13926 - FriBidi soft-ok unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not run bidi algorithms
 * or call FriBidi/libc. No parent wires.
 */
uint32_t
gj_fribidi_ok_u_13926(void)
{
	(void)NULL;
	return b13926_fribidi_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fribidi_ok_u_13926(void)
    __attribute__((alias("gj_fribidi_ok_u_13926")));
