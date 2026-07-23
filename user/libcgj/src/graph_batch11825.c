/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11825: Pango soft-ok unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pango_ok_u_11825(void);
 *     - Always returns 0. Soft stub for fontconfig/freetype soft-id
 *       continuum; Pango is intentionally not reported ok.
 *   uint32_t __gj_pango_ok_u_11825  (alias)
 *   __libcgj_batch11825_marker = "libcgj-batch11825"
 *
 * Exclusive continuum CREATE-ONLY (11821-11830: fontconfig/freetype
 * soft id stubs — fontconfig_ok_u_11821, freetype_ok_u_11822,
 * harfbuzz_ok_u_11823, cairo_ok_u_11824, pango_ok_u_11825,
 * fribidi_ok_u_11826, graphite_ok_u_11827, font_match_ok_u_11828,
 * font_soft_ready_u_11829, batch_id_11830).
 * Unique surface only; no multi-def. Distinct from
 * gj_pango_ok_u_11025 (batch11025). No parent wires. No __int128.
 * No Pango implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11825_marker[] = "libcgj-batch11825";

/* Soft Pango-ok lamp: always off for this continuum. */
#define B11825_PANGO_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11825_pango_ok(void)
{
	return B11825_PANGO_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pango_ok_u_11825 - Pango soft-ok unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not create layouts or
 * call Pango/libc. No parent wires.
 */
uint32_t
gj_pango_ok_u_11825(void)
{
	(void)NULL;
	return b11825_pango_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pango_ok_u_11825(void)
    __attribute__((alias("gj_pango_ok_u_11825")));
