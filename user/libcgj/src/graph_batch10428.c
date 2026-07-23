/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10428: font_match ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_font_match_ok_u_10428(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       font-match runtime probe for the font soft continuum.
 *   uint32_t __gj_font_match_ok_u_10428  (alias)
 *   __libcgj_batch10428_marker = "libcgj-batch10428"
 *
 * Exclusive continuum CREATE-ONLY (10421-10430: font soft
 * id stubs — fontconfig_ok_u_10421, freetype_ok_u_10422,
 * harfbuzz_ok_u_10423, cairo_ok_u_10424, pango_ok_u_10425,
 * fribidi_ok_u_10426, graphite_ok_u_10427, font_match_ok_u_10428,
 * font_soft_ready_u_10429, batch_id_10430). Unique surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10428_marker[] = "libcgj-batch10428";

/* Soft font_match-ok lamp: always off (not a real runtime probe). */
#define B10428_FONT_MATCH_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10428_ok(void)
{
	return B10428_FONT_MATCH_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_font_match_ok_u_10428 - font_match ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not run FcFontMatch
 * or call libc. No parent wires.
 */
uint32_t
gj_font_match_ok_u_10428(void)
{
	(void)NULL;
	return b10428_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_font_match_ok_u_10428(void)
    __attribute__((alias("gj_font_match_ok_u_10428")));
