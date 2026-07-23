/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11023: HarfBuzz soft-ok unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_harfbuzz_ok_u_11023(void);
 *     - Always returns 0. Soft stub for fontconfig/freetype soft-id
 *       continuum; HarfBuzz is intentionally not reported ok.
 *   uint32_t __gj_harfbuzz_ok_u_11023  (alias)
 *   __libcgj_batch11023_marker = "libcgj-batch11023"
 *
 * Exclusive continuum CREATE-ONLY (11021-11030: fontconfig/freetype
 * soft id stubs — fontconfig_ok_u_11021, freetype_ok_u_11022,
 * harfbuzz_ok_u_11023, cairo_ok_u_11024, pango_ok_u_11025,
 * fribidi_ok_u_11026, graphite_ok_u_11027, font_match_ok_u_11028,
 * font_soft_ready_u_11029, batch_id_11030).
 * Unique surface only; no multi-def. Distinct from
 * gj_harfbuzz_ok_u_10123 (batch10123). No parent wires. No __int128.
 * No HarfBuzz implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11023_marker[] = "libcgj-batch11023";

/* Soft HarfBuzz-ok lamp: always off for this continuum. */
#define B11023_HARFBUZZ_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11023_harfbuzz_ok(void)
{
	return B11023_HARFBUZZ_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_harfbuzz_ok_u_11023 - HarfBuzz soft-ok unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not shape text or call
 * HarfBuzz/libc. No parent wires.
 */
uint32_t
gj_harfbuzz_ok_u_11023(void)
{
	(void)NULL;
	return b11023_harfbuzz_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_harfbuzz_ok_u_11023(void)
    __attribute__((alias("gj_harfbuzz_ok_u_11023")));
