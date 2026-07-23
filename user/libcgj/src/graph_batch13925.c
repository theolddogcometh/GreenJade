/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13925: Pango soft-ok unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_pango_ok_u_13925(void);
 *     - Always returns 0. Soft stub for fontconfig/freetype soft-id
 *       continuum; Pango is intentionally not reported ok.
 *   uint32_t __gj_pango_ok_u_13925  (alias)
 *   __libcgj_batch13925_marker = "libcgj-batch13925"
 *
 * Exclusive continuum CREATE-ONLY (13921-13930: fontconfig/freetype
 * soft id stubs — fontconfig_ok_u_13921, freetype_ok_u_13922,
 * harfbuzz_ok_u_13923, cairo_ok_u_13924, pango_ok_u_13925,
 * fribidi_ok_u_13926, graphite_ok_u_13927, font_match_ok_u_13928,
 * font_soft_ready_u_13929→1, batch_id_13930→13930; others →0).
 * Unique surface only; no multi-def. Distinct from
 * gj_pango_ok_u_13725 (batch13725), gj_pango_ok_u_13524 (batch13524),
 * gj_pango_ok_u_13325 (batch13325) and gj_pango_ok_u_13125
 * (batch13125). No parent wires. No __int128. No Pango
 * implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13925_marker[] = "libcgj-batch13925";

/* Soft Pango-ok lamp: always off for this continuum. */
#define B13925_PANGO_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13925_pango_ok(void)
{
	return B13925_PANGO_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pango_ok_u_13925 - Pango soft-ok unit flag.
 *
 * Always returns 0. Soft pure-data stub; does not create PangoContext
 * or call Pango/libc. No parent wires.
 */
uint32_t
gj_pango_ok_u_13925(void)
{
	(void)NULL;
	return b13925_pango_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pango_ok_u_13925(void)
    __attribute__((alias("gj_pango_ok_u_13925")));
