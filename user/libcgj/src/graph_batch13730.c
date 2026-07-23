/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13730: font soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13730(void);
 *     - Returns the compile-time graph batch number for this TU (13730).
 *   uint32_t __gj_batch_id_13730  (alias)
 *   __libcgj_batch13730_marker = "libcgj-batch13730"
 *
 * Exclusive continuum CREATE-ONLY (13721-13730: fontconfig/freetype
 * soft id stubs — fontconfig_ok_u_13721, freetype_ok_u_13722,
 * harfbuzz_ok_u_13723, cairo_ok_u_13724, pango_ok_u_13725,
 * fribidi_ok_u_13726, graphite_ok_u_13727, font_match_ok_u_13728,
 * font_soft_ready_u_13729→1, batch_id_13730→13730; others →0).
 * Unique surfaces only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * Distinct from gj_batch_id_13530 (batch13530), gj_batch_id_13330
 * (batch13330) and gj_batch_id_13130 (batch13130). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13730_marker[] = "libcgj-batch13730";

#define B13730_BATCH_ID  13730u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13730_id(void)
{
	return B13730_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13730 - report this TU's graph batch number.
 *
 * Always returns 13730. Useful for link-time presence checks and
 * runtime graph revision probes without string compares on the
 * marker. No parent wires.
 */
uint32_t
gj_batch_id_13730(void)
{
	(void)NULL;
	return b13730_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13730(void)
    __attribute__((alias("gj_batch_id_13730")));
