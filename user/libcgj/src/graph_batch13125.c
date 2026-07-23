/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13125: Pango soft-ok unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pango_ok_u_13125(void);
 *     - Always returns 0. Soft stub for fontconfig/freetype soft-id
 *       continuum; Pango is intentionally not reported ok.
 *   uint32_t __gj_pango_ok_u_13125  (alias)
 *   __libcgj_batch13125_marker = "libcgj-batch13125"
 *
 * Exclusive continuum CREATE-ONLY (13121-13130: fontconfig/freetype
 * soft id stubs — fontconfig_ok_u_13121, freetype_ok_u_13122,
 * harfbuzz_ok_u_13123, cairo_ok_u_13124, pango_ok_u_13125,
 * fribidi_ok_u_13126, graphite_ok_u_13127, font_match_ok_u_13128,
 * font_soft_ready_u_13129→1, batch_id_13130→13130; others →0).
 * Unique surface only; no multi-def. Distinct from
 * gj_pango_ok_u_12925 (batch12925), gj_pango_ok_u_12725 (batch12725),
 * gj_pango_ok_u_12525 (batch12525), gj_pango_ok_u_12325 (batch12325)
 * and gj_pango_ok_u_12125 (batch12125). No parent wires. No __int128.
 * No Pango implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13125_marker[] = "libcgj-batch13125";

/* Soft Pango-ok lamp: always off for this continuum. */
#define B13125_PANGO_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13125_pango_ok(void)
{
	return B13125_PANGO_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pango_ok_u_13125 - Pango soft-ok unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not create layouts or
 * call Pango/libc. No parent wires.
 */
uint32_t
gj_pango_ok_u_13125(void)
{
	(void)NULL;
	return b13125_pango_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pango_ok_u_13125(void)
    __attribute__((alias("gj_pango_ok_u_13125")));
