/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10625: Pango soft-ok unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pango_ok_u_10625(void);
 *     - Always returns 0. Soft stub for fontconfig/freetype soft-id
 *       continuum; Pango is intentionally not reported ok.
 *   uint32_t __gj_pango_ok_u_10625  (alias)
 *   __libcgj_batch10625_marker = "libcgj-batch10625"
 *
 * Exclusive continuum CREATE-ONLY (10621-10630: fontconfig/freetype
 * soft id stubs — fontconfig_ok_u_10621, freetype_ok_u_10622,
 * harfbuzz_ok_u_10623, cairo_ok_u_10624, pango_ok_u_10625,
 * fribidi_ok_u_10626, graphite_ok_u_10627, font_match_ok_u_10628,
 * font_soft_ready_u_10629, batch_id_10630).
 * Unique surface only; no multi-def. Distinct from
 * gj_pango_ok_u_10125 (batch10125). No parent wires. No __int128.
 * No Pango implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10625_marker[] = "libcgj-batch10625";

/* Soft Pango-ok lamp: always off for this continuum. */
#define B10625_PANGO_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10625_pango_ok(void)
{
	return B10625_PANGO_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pango_ok_u_10625 - Pango soft-ok unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not create layouts or
 * call Pango/libc. No parent wires.
 */
uint32_t
gj_pango_ok_u_10625(void)
{
	(void)NULL;
	return b10625_pango_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pango_ok_u_10625(void)
    __attribute__((alias("gj_pango_ok_u_10625")));
