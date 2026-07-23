/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13528: fc_match ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_fc_match_ok_u_13528(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       FcFontMatch runtime probe for the font soft continuum.
 *   uint32_t __gj_fc_match_ok_u_13528  (alias)
 *   __libcgj_batch13528_marker = "libcgj-batch13528"
 *
 * Exclusive continuum CREATE-ONLY (13521-13530: font soft id stubs —
 * fontconfig_ok_u_13521, freetype_ok_u_13522, harfbuzz_ok_u_13523,
 * pango_ok_u_13524, cairo_ft_ok_u_13525, fc_pattern_ok_u_13526,
 * fc_cache_ok_u_13527, fc_match_ok_u_13528,
 * font_soft_ready_u_13529, batch_id_13530→13530). Unique surface only;
 * no multi-def. Ok units remain 0; ready→1. Distinct from sibling font
 * soft ok_u stubs in this wave. No parent wires. No __int128. No
 * fontconfig FcFontMatch implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13528_marker[] = "libcgj-batch13528";

/* Soft fc_match-ok lamp: always off (not a real runtime probe). */
#define B13528_FC_MATCH_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13528_ok(void)
{
	return B13528_FC_MATCH_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fc_match_ok_u_13528 - fontconfig FcFontMatch ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not match fonts or call
 * libc. No parent wires.
 */
uint32_t
gj_fc_match_ok_u_13528(void)
{
	(void)NULL;
	return b13528_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fc_match_ok_u_13528(void)
    __attribute__((alias("gj_fc_match_ok_u_13528")));
