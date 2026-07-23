/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13930: font soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13930(void);
 *     - Returns the compile-time graph batch number for this TU (13930).
 *   uint32_t __gj_batch_id_13930  (alias)
 *   __libcgj_batch13930_marker = "libcgj-batch13930"
 *
 * Exclusive continuum CREATE-ONLY (13921-13930: fontconfig/freetype
 * soft id stubs — fontconfig_ok_u_13921, freetype_ok_u_13922,
 * harfbuzz_ok_u_13923, cairo_ok_u_13924, pango_ok_u_13925,
 * fribidi_ok_u_13926, graphite_ok_u_13927, font_match_ok_u_13928,
 * font_soft_ready_u_13929→1, batch_id_13930→13930; others →0).
 * Unique surfaces only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * Distinct from gj_batch_id_13730 (batch13730), gj_batch_id_13530
 * (batch13530), gj_batch_id_13330 (batch13330) and gj_batch_id_13130
 * (batch13130). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13930_marker[] = "libcgj-batch13930";

#define B13930_BATCH_ID  13930u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13930_id(void)
{
	return B13930_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13930 - report this TU's graph batch number.
 *
 * Always returns 13930. Useful for link-time presence checks and
 * runtime graph revision probes without string compares on the
 * marker. No parent wires.
 */
uint32_t
gj_batch_id_13930(void)
{
	(void)NULL;
	return b13930_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13930(void)
    __attribute__((alias("gj_batch_id_13930")));
