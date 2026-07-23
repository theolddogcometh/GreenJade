/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11830: font soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_11830(void);
 *     - Returns the compile-time graph batch number for this TU (11830).
 *   uint32_t __gj_batch_id_11830  (alias)
 *   __libcgj_batch11830_marker = "libcgj-batch11830"
 *
 * Exclusive continuum CREATE-ONLY (11821-11830: fontconfig/freetype
 * soft id stubs — fontconfig_ok_u_11821, freetype_ok_u_11822,
 * harfbuzz_ok_u_11823, cairo_ok_u_11824, pango_ok_u_11825,
 * fribidi_ok_u_11826, graphite_ok_u_11827, font_match_ok_u_11828,
 * font_soft_ready_u_11829, batch_id_11830).
 * Unique surfaces only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11830_marker[] = "libcgj-batch11830";

#define B11830_BATCH_ID  11830u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11830_id(void)
{
	return B11830_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_11830 - report this TU's graph batch number.
 *
 * Always returns 11830.
 */
uint32_t
gj_batch_id_11830(void)
{
	(void)NULL;
	return b11830_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_11830(void)
    __attribute__((alias("gj_batch_id_11830")));
