/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11230: font soft id continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_11230(void);
 *     - Returns the compile-time graph batch number for this TU (11230).
 *   uint32_t __gj_batch_id_11230  (alias)
 *   __libcgj_batch11230_marker = "libcgj-batch11230"
 *
 * Exclusive continuum CREATE-ONLY (11221-11230: font soft
 * id stubs — fontconfig_ok_u_11221, freetype_ok_u_11222,
 * harfbuzz_ok_u_11223, cairo_ok_u_11224, pango_ok_u_11225,
 * fribidi_ok_u_11226, graphite_ok_u_11227, font_match_ok_u_11228,
 * font_soft_ready_u_11229, batch_id_11230). Unique surfaces only;
 * no multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. Distinct from
 * gj_batch_id_10430 / gj_batch_id_10130. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11230_marker[] = "libcgj-batch11230";

#define B11230_BATCH_ID  11230u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11230_id(void)
{
	return B11230_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_11230 - report this TU's graph batch number.
 *
 * Always returns 11230.
 */
uint32_t
gj_batch_id_11230(void)
{
	(void)NULL;
	return b11230_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_11230(void)
    __attribute__((alias("gj_batch_id_11230")));
