/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12730: font soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_12730(void);
 *     - Returns the compile-time graph batch number for this TU (12730).
 *   uint32_t __gj_batch_id_12730  (alias)
 *   __libcgj_batch12730_marker = "libcgj-batch12730"
 *
 * Exclusive continuum CREATE-ONLY (12721-12730: fontconfig/freetype
 * soft id stubs — fontconfig_ok_u_12721, freetype_ok_u_12722,
 * harfbuzz_ok_u_12723, cairo_ok_u_12724, pango_ok_u_12725,
 * fribidi_ok_u_12726, graphite_ok_u_12727, font_match_ok_u_12728,
 * font_soft_ready_u_12729→1, batch_id_12730→12730; others →0).
 * Unique surfaces only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * Distinct from gj_batch_id_12530 (batch12530), gj_batch_id_12330
 * (batch12330) and gj_batch_id_12130 (batch12130). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12730_marker[] = "libcgj-batch12730";

#define B12730_BATCH_ID  12730u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12730_id(void)
{
	return B12730_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_12730 - report this TU's graph batch number.
 *
 * Always returns 12730.
 */
uint32_t
gj_batch_id_12730(void)
{
	(void)NULL;
	return b12730_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_12730(void)
    __attribute__((alias("gj_batch_id_12730")));
