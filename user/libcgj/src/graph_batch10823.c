/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10823: HarfBuzz soft-ok unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_harfbuzz_ok_u_10823(void);
 *     - Always returns 0. Soft stub for fontconfig/freetype soft-id
 *       continuum; HarfBuzz is intentionally not reported ok.
 *   uint32_t __gj_harfbuzz_ok_u_10823  (alias)
 *   __libcgj_batch10823_marker = "libcgj-batch10823"
 *
 * Exclusive continuum CREATE-ONLY (10821-10830: fontconfig/freetype
 * soft id stubs — fontconfig_ok_u_10821, freetype_ok_u_10822,
 * harfbuzz_ok_u_10823, cairo_ok_u_10824, pango_ok_u_10825,
 * fribidi_ok_u_10826, graphite_ok_u_10827, font_match_ok_u_10828,
 * font_soft_ready_u_10829, batch_id_10830).
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

const char __libcgj_batch10823_marker[] = "libcgj-batch10823";

/* Soft HarfBuzz-ok lamp: always off for this continuum. */
#define B10823_HARFBUZZ_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10823_harfbuzz_ok(void)
{
	return B10823_HARFBUZZ_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_harfbuzz_ok_u_10823 - HarfBuzz soft-ok unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not shape text or call
 * HarfBuzz/libc. No parent wires.
 */
uint32_t
gj_harfbuzz_ok_u_10823(void)
{
	(void)NULL;
	return b10823_harfbuzz_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_harfbuzz_ok_u_10823(void)
    __attribute__((alias("gj_harfbuzz_ok_u_10823")));
