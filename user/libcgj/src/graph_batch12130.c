/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12130: font soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_12130(void);
 *     - Returns the compile-time graph batch number for this TU (12130).
 *   uint32_t __gj_batch_id_12130  (alias)
 *   __libcgj_batch12130_marker = "libcgj-batch12130"
 *
 * Exclusive continuum CREATE-ONLY (12121-12130: fontconfig/freetype
 * soft id stubs — fontconfig_ok_u_12121, freetype_ok_u_12122,
 * harfbuzz_ok_u_12123, cairo_ok_u_12124, pango_ok_u_12125,
 * fribidi_ok_u_12126, graphite_ok_u_12127, font_match_ok_u_12128,
 * font_soft_ready_u_12129, batch_id_12130).
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

const char __libcgj_batch12130_marker[] = "libcgj-batch12130";

#define B12130_BATCH_ID  12130u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12130_id(void)
{
	return B12130_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_12130 - report this TU's graph batch number.
 *
 * Always returns 12130.
 */
uint32_t
gj_batch_id_12130(void)
{
	(void)NULL;
	return b12130_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_12130(void)
    __attribute__((alias("gj_batch_id_12130")));
