/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11427: graphite ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_graphite_ok_u_11427(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Graphite2 runtime probe for the font soft continuum.
 *   uint32_t __gj_graphite_ok_u_11427  (alias)
 *   __libcgj_batch11427_marker = "libcgj-batch11427"
 *
 * Exclusive continuum CREATE-ONLY (11421-11430: font soft
 * id stubs — fontconfig_ok_u_11421, freetype_ok_u_11422,
 * harfbuzz_ok_u_11423, cairo_ok_u_11424, pango_ok_u_11425,
 * fribidi_ok_u_11426, graphite_ok_u_11427, font_match_ok_u_11428,
 * font_soft_ready_u_11429, batch_id_11430). Unique surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11427_marker[] = "libcgj-batch11427";

/* Soft graphite-ok lamp: always off (not a real runtime probe). */
#define B11427_GRAPHITE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11427_ok(void)
{
	return B11427_GRAPHITE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_graphite_ok_u_11427 - graphite ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe Graphite2
 * presence or call libc. No parent wires.
 */
uint32_t
gj_graphite_ok_u_11427(void)
{
	(void)NULL;
	return b11427_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_graphite_ok_u_11427(void)
    __attribute__((alias("gj_graphite_ok_u_11427")));
