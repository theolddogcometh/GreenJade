/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10123: HarfBuzz soft-ok unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_harfbuzz_ok_u_10123(void);
 *     - Always returns 0. Soft stub for fontconfig/freetype soft-id
 *       continuum; HarfBuzz is intentionally not reported ok.
 *   uint32_t __gj_harfbuzz_ok_u_10123  (alias)
 *   __libcgj_batch10123_marker = "libcgj-batch10123"
 *
 * Exclusive continuum CREATE-ONLY (10121-10130: fontconfig/freetype
 * soft id stubs — fontconfig_ok_u_10121, freetype_ok_u_10122,
 * harfbuzz_ok_u_10123, cairo_ok_u_10124, pango_ok_u_10125,
 * fribidi_ok_u_10126, graphite_ok_u_10127, font_match_ok_u_10128,
 * font_soft_ready_u_10129, batch_id_10130).
 * Unique surface only; no multi-def. Distinct from
 * gj_hb_shape_ok_u_9058 (batch9058). No parent wires. No __int128.
 * No HarfBuzz implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10123_marker[] = "libcgj-batch10123";

/* Soft HarfBuzz-ok lamp: always off for this continuum. */
#define B10123_HARFBUZZ_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10123_harfbuzz_ok(void)
{
	return B10123_HARFBUZZ_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_harfbuzz_ok_u_10123 - HarfBuzz soft-ok unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not shape text or call
 * HarfBuzz/libc. No parent wires.
 */
uint32_t
gj_harfbuzz_ok_u_10123(void)
{
	(void)NULL;
	return b10123_harfbuzz_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_harfbuzz_ok_u_10123(void)
    __attribute__((alias("gj_harfbuzz_ok_u_10123")));
