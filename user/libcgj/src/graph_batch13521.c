/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13521: fontconfig ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_fontconfig_ok_u_13521(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       fontconfig runtime probe for the font soft continuum.
 *   uint32_t __gj_fontconfig_ok_u_13521  (alias)
 *   __libcgj_batch13521_marker = "libcgj-batch13521"
 *
 * Exclusive continuum CREATE-ONLY (13521-13530: font soft id stubs —
 * fontconfig_ok_u_13521, freetype_ok_u_13522, harfbuzz_ok_u_13523,
 * pango_ok_u_13524, cairo_ft_ok_u_13525, fc_pattern_ok_u_13526,
 * fc_cache_ok_u_13527, fc_match_ok_u_13528,
 * font_soft_ready_u_13529, batch_id_13530→13530). Unique surface only;
 * no multi-def. Ok units remain 0; ready→1. No parent wires. No __int128.
 * No fontconfig implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13521_marker[] = "libcgj-batch13521";

/* Soft fontconfig-ok lamp: always off (not a real runtime probe). */
#define B13521_FONTCONFIG_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13521_ok(void)
{
	return B13521_FONTCONFIG_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fontconfig_ok_u_13521 - fontconfig ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe fontconfig
 * library presence or call libc. No parent wires.
 */
uint32_t
gj_fontconfig_ok_u_13521(void)
{
	(void)NULL;
	return b13521_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fontconfig_ok_u_13521(void)
    __attribute__((alias("gj_fontconfig_ok_u_13521")));
