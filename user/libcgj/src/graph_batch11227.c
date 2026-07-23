/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11227: graphite ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_graphite_ok_u_11227(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Graphite2 runtime probe for the font soft continuum.
 *   uint32_t __gj_graphite_ok_u_11227  (alias)
 *   __libcgj_batch11227_marker = "libcgj-batch11227"
 *
 * Exclusive continuum CREATE-ONLY (11221-11230: font soft
 * id stubs — fontconfig_ok_u_11221, freetype_ok_u_11222,
 * harfbuzz_ok_u_11223, cairo_ok_u_11224, pango_ok_u_11225,
 * fribidi_ok_u_11226, graphite_ok_u_11227, font_match_ok_u_11228,
 * font_soft_ready_u_11229, batch_id_11230). Unique surface only;
 * no multi-def. Distinct from gj_graphite_ok_u_10427 /
 * gj_graphite_ok_u_10127. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11227_marker[] = "libcgj-batch11227";

/* Soft graphite-ok lamp: always off (not a real runtime probe). */
#define B11227_GRAPHITE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11227_ok(void)
{
	return B11227_GRAPHITE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_graphite_ok_u_11227 - graphite ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe Graphite2
 * presence or call libc. No parent wires.
 */
uint32_t
gj_graphite_ok_u_11227(void)
{
	(void)NULL;
	return b11227_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_graphite_ok_u_11227(void)
    __attribute__((alias("gj_graphite_ok_u_11227")));
