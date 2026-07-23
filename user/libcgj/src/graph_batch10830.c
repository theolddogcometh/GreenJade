/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10830: font soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10830(void);
 *     - Returns the compile-time graph batch number for this TU (10830).
 *   uint32_t __gj_batch_id_10830  (alias)
 *   __libcgj_batch10830_marker = "libcgj-batch10830"
 *
 * Exclusive continuum CREATE-ONLY (10821-10830: fontconfig/freetype
 * soft id stubs — fontconfig_ok_u_10821, freetype_ok_u_10822,
 * harfbuzz_ok_u_10823, cairo_ok_u_10824, pango_ok_u_10825,
 * fribidi_ok_u_10826, graphite_ok_u_10827, font_match_ok_u_10828,
 * font_soft_ready_u_10829, batch_id_10830).
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

const char __libcgj_batch10830_marker[] = "libcgj-batch10830";

#define B10830_BATCH_ID  10830u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10830_id(void)
{
	return B10830_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10830 - report this TU's graph batch number.
 *
 * Always returns 10830.
 */
uint32_t
gj_batch_id_10830(void)
{
	(void)NULL;
	return b10830_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10830(void)
    __attribute__((alias("gj_batch_id_10830")));
