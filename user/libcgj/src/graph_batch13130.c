/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13130: font soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13130(void);
 *     - Returns the compile-time graph batch number for this TU (13130).
 *   uint32_t __gj_batch_id_13130  (alias)
 *   __libcgj_batch13130_marker = "libcgj-batch13130"
 *
 * Exclusive continuum CREATE-ONLY (13121-13130: fontconfig/freetype
 * soft id stubs — fontconfig_ok_u_13121, freetype_ok_u_13122,
 * harfbuzz_ok_u_13123, cairo_ok_u_13124, pango_ok_u_13125,
 * fribidi_ok_u_13126, graphite_ok_u_13127, font_match_ok_u_13128,
 * font_soft_ready_u_13129→1, batch_id_13130→13130; others →0).
 * Unique surfaces only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * Distinct from gj_batch_id_12930 (batch12930), gj_batch_id_12730
 * (batch12730), gj_batch_id_12530 (batch12530), gj_batch_id_12330
 * (batch12330) and gj_batch_id_12130 (batch12130). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13130_marker[] = "libcgj-batch13130";

#define B13130_BATCH_ID  13130u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13130_id(void)
{
	return B13130_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13130 - report this TU's graph batch number.
 *
 * Always returns 13130.
 */
uint32_t
gj_batch_id_13130(void)
{
	(void)NULL;
	return b13130_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13130(void)
    __attribute__((alias("gj_batch_id_13130")));
