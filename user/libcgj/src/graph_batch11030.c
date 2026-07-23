/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11030: font soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_11030(void);
 *     - Returns the compile-time graph batch number for this TU (11030).
 *   uint32_t __gj_batch_id_11030  (alias)
 *   __libcgj_batch11030_marker = "libcgj-batch11030"
 *
 * Exclusive continuum CREATE-ONLY (11021-11030: fontconfig/freetype
 * soft id stubs — fontconfig_ok_u_11021, freetype_ok_u_11022,
 * harfbuzz_ok_u_11023, cairo_ok_u_11024, pango_ok_u_11025,
 * fribidi_ok_u_11026, graphite_ok_u_11027, font_match_ok_u_11028,
 * font_soft_ready_u_11029, batch_id_11030).
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

const char __libcgj_batch11030_marker[] = "libcgj-batch11030";

#define B11030_BATCH_ID  11030u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11030_id(void)
{
	return B11030_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_11030 - report this TU's graph batch number.
 *
 * Always returns 11030.
 */
uint32_t
gj_batch_id_11030(void)
{
	(void)NULL;
	return b11030_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_11030(void)
    __attribute__((alias("gj_batch_id_11030")));
