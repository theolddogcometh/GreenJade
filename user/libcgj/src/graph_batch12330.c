/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12330: font soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_12330(void);
 *     - Returns the compile-time graph batch number for this TU (12330).
 *   uint32_t __gj_batch_id_12330  (alias)
 *   __libcgj_batch12330_marker = "libcgj-batch12330"
 *
 * Exclusive continuum CREATE-ONLY (12321-12330: fontconfig/freetype
 * soft id stubs — fontconfig_ok_u_12321, freetype_ok_u_12322,
 * harfbuzz_ok_u_12323, cairo_ok_u_12324, pango_ok_u_12325,
 * fribidi_ok_u_12326, graphite_ok_u_12327, font_match_ok_u_12328,
 * font_soft_ready_u_12329, batch_id_12330).
 * Unique surfaces only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * Distinct from gj_batch_id_12130 (batch12130). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12330_marker[] = "libcgj-batch12330";

#define B12330_BATCH_ID  12330u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12330_id(void)
{
	return B12330_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_12330 - report this TU's graph batch number.
 *
 * Always returns 12330.
 */
uint32_t
gj_batch_id_12330(void)
{
	(void)NULL;
	return b12330_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_12330(void)
    __attribute__((alias("gj_batch_id_12330")));
