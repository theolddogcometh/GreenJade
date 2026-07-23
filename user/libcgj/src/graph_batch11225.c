/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11225: pango ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pango_ok_u_11225(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Pango runtime probe for the font soft continuum.
 *   uint32_t __gj_pango_ok_u_11225  (alias)
 *   __libcgj_batch11225_marker = "libcgj-batch11225"
 *
 * Exclusive continuum CREATE-ONLY (11221-11230: font soft
 * id stubs — fontconfig_ok_u_11221, freetype_ok_u_11222,
 * harfbuzz_ok_u_11223, cairo_ok_u_11224, pango_ok_u_11225,
 * fribidi_ok_u_11226, graphite_ok_u_11227, font_match_ok_u_11228,
 * font_soft_ready_u_11229, batch_id_11230). Unique surface only;
 * no multi-def. Distinct from gj_pango_ok_u_10425 /
 * gj_pango_ok_u_10125. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11225_marker[] = "libcgj-batch11225";

/* Soft pango-ok lamp: always off (not a real runtime probe). */
#define B11225_PANGO_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11225_ok(void)
{
	return B11225_PANGO_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pango_ok_u_11225 - pango ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe Pango
 * presence or call libc. No parent wires.
 */
uint32_t
gj_pango_ok_u_11225(void)
{
	(void)NULL;
	return b11225_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pango_ok_u_11225(void)
    __attribute__((alias("gj_pango_ok_u_11225")));
