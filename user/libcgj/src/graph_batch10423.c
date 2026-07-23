/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10423: harfbuzz ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_harfbuzz_ok_u_10423(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       HarfBuzz runtime probe for the font soft continuum.
 *   uint32_t __gj_harfbuzz_ok_u_10423  (alias)
 *   __libcgj_batch10423_marker = "libcgj-batch10423"
 *
 * Exclusive continuum CREATE-ONLY (10421-10430: font soft
 * id stubs — fontconfig_ok_u_10421, freetype_ok_u_10422,
 * harfbuzz_ok_u_10423, cairo_ok_u_10424, pango_ok_u_10425,
 * fribidi_ok_u_10426, graphite_ok_u_10427, font_match_ok_u_10428,
 * font_soft_ready_u_10429, batch_id_10430). Unique surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10423_marker[] = "libcgj-batch10423";

/* Soft harfbuzz-ok lamp: always off (not a real runtime probe). */
#define B10423_HARFBUZZ_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10423_ok(void)
{
	return B10423_HARFBUZZ_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_harfbuzz_ok_u_10423 - harfbuzz ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe HarfBuzz
 * presence or call libc. No parent wires.
 */
uint32_t
gj_harfbuzz_ok_u_10423(void)
{
	(void)NULL;
	return b10423_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_harfbuzz_ok_u_10423(void)
    __attribute__((alias("gj_harfbuzz_ok_u_10423")));
