/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13330: font soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13330(void);
 *     - Returns the compile-time graph batch number for this TU (13330).
 *   uint32_t __gj_batch_id_13330  (alias)
 *   __libcgj_batch13330_marker = "libcgj-batch13330"
 *
 * Exclusive continuum CREATE-ONLY (13321-13330: fontconfig/freetype
 * soft id stubs — fontconfig_ok_u_13321, freetype_ok_u_13322,
 * harfbuzz_ok_u_13323, cairo_ok_u_13324, pango_ok_u_13325,
 * fribidi_ok_u_13326, graphite_ok_u_13327, font_match_ok_u_13328,
 * font_soft_ready_u_13329, batch_id_13330).
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

const char __libcgj_batch13330_marker[] = "libcgj-batch13330";

#define B13330_BATCH_ID  13330u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13330_id(void)
{
	return B13330_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13330 - report this TU's graph batch number.
 *
 * Always returns 13330. Useful for link-time presence checks and
 * runtime graph revision probes without string compares on the
 * marker. No parent wires.
 */
uint32_t
gj_batch_id_13330(void)
{
	(void)NULL;
	return b13330_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13330(void)
    __attribute__((alias("gj_batch_id_13330")));
