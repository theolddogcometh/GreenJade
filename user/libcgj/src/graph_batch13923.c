/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13923: HarfBuzz soft-ok unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_harfbuzz_ok_u_13923(void);
 *     - Always returns 0. Soft stub for fontconfig/freetype soft-id
 *       continuum; HarfBuzz is intentionally not reported ok.
 *   uint32_t __gj_harfbuzz_ok_u_13923  (alias)
 *   __libcgj_batch13923_marker = "libcgj-batch13923"
 *
 * Exclusive continuum CREATE-ONLY (13921-13930: fontconfig/freetype
 * soft id stubs — fontconfig_ok_u_13921, freetype_ok_u_13922,
 * harfbuzz_ok_u_13923, cairo_ok_u_13924, pango_ok_u_13925,
 * fribidi_ok_u_13926, graphite_ok_u_13927, font_match_ok_u_13928,
 * font_soft_ready_u_13929→1, batch_id_13930→13930; others →0).
 * Unique surface only; no multi-def. Distinct from
 * gj_harfbuzz_ok_u_13723 (batch13723), gj_harfbuzz_ok_u_13523
 * (batch13523), gj_harfbuzz_ok_u_13323 (batch13323) and
 * gj_hb_shape_ok_u_9058 (batch9058). No parent wires. No __int128.
 * No HarfBuzz implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13923_marker[] = "libcgj-batch13923";

/* Soft HarfBuzz-ok lamp: always off for this continuum. */
#define B13923_HARFBUZZ_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13923_harfbuzz_ok(void)
{
	return B13923_HARFBUZZ_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_harfbuzz_ok_u_13923 - HarfBuzz soft-ok unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not shape text or call
 * HarfBuzz/libc. No parent wires.
 */
uint32_t
gj_harfbuzz_ok_u_13923(void)
{
	(void)NULL;
	return b13923_harfbuzz_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_harfbuzz_ok_u_13923(void)
    __attribute__((alias("gj_harfbuzz_ok_u_13923")));
