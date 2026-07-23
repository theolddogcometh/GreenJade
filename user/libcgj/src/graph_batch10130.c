/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10130: font soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10130(void);
 *     - Returns the compile-time graph batch number for this TU (10130).
 *   uint32_t __gj_batch_id_10130  (alias)
 *   __libcgj_batch10130_marker = "libcgj-batch10130"
 *
 * Exclusive continuum CREATE-ONLY (10121-10130: fontconfig/freetype
 * soft id stubs — fontconfig_ok_u_10121, freetype_ok_u_10122,
 * harfbuzz_ok_u_10123, cairo_ok_u_10124, pango_ok_u_10125,
 * fribidi_ok_u_10126, graphite_ok_u_10127, font_match_ok_u_10128,
 * font_soft_ready_u_10129, batch_id_10130).
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

const char __libcgj_batch10130_marker[] = "libcgj-batch10130";

#define B10130_BATCH_ID  10130u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10130_id(void)
{
	return B10130_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10130 - report this TU's graph batch number.
 *
 * Always returns 10130.
 */
uint32_t
gj_batch_id_10130(void)
{
	(void)NULL;
	return b10130_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10130(void)
    __attribute__((alias("gj_batch_id_10130")));
