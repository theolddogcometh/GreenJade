/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12930: font soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_12930(void);
 *     - Returns the compile-time graph batch number for this TU (12930).
 *   uint32_t __gj_batch_id_12930  (alias)
 *   __libcgj_batch12930_marker = "libcgj-batch12930"
 *
 * Exclusive continuum CREATE-ONLY (12921-12930: fontconfig/freetype
 * soft id stubs — fontconfig_ok_u_12921, freetype_ok_u_12922,
 * harfbuzz_ok_u_12923, cairo_ok_u_12924, pango_ok_u_12925,
 * fribidi_ok_u_12926, graphite_ok_u_12927, font_match_ok_u_12928,
 * font_soft_ready_u_12929→1, batch_id_12930→12930; others →0).
 * Unique surfaces only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * Distinct from gj_batch_id_12730 (batch12730), gj_batch_id_12530
 * (batch12530), gj_batch_id_12330 (batch12330) and gj_batch_id_12130
 * (batch12130). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12930_marker[] = "libcgj-batch12930";

#define B12930_BATCH_ID  12930u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12930_id(void)
{
	return B12930_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_12930 - report this TU's graph batch number.
 *
 * Always returns 12930.
 */
uint32_t
gj_batch_id_12930(void)
{
	(void)NULL;
	return b12930_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_12930(void)
    __attribute__((alias("gj_batch_id_12930")));
