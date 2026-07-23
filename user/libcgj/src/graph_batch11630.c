/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11630: font soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_11630(void);
 *     - Returns the compile-time graph batch number for this TU (11630).
 *   uint32_t __gj_batch_id_11630  (alias)
 *   __libcgj_batch11630_marker = "libcgj-batch11630"
 *
 * Exclusive continuum CREATE-ONLY (11621-11630: fontconfig/freetype
 * soft id stubs — fontconfig_ok_u_11621, freetype_ok_u_11622,
 * harfbuzz_ok_u_11623, cairo_ok_u_11624, pango_ok_u_11625,
 * fribidi_ok_u_11626, graphite_ok_u_11627, font_match_ok_u_11628,
 * font_soft_ready_u_11629, batch_id_11630).
 * Unique surfaces only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * Distinct from gj_batch_id_10130 (batch10130). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11630_marker[] = "libcgj-batch11630";

#define B11630_BATCH_ID  11630u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11630_id(void)
{
	return B11630_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_11630 - report this TU's graph batch number.
 *
 * Always returns 11630.
 */
uint32_t
gj_batch_id_11630(void)
{
	(void)NULL;
	return b11630_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_11630(void)
    __attribute__((alias("gj_batch_id_11630")));
