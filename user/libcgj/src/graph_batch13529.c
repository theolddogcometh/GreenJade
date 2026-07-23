/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13529: soft font continuum ready.
 *
 * Surface (unique symbols):
 *   uint32_t gj_font_soft_ready_u_13529(void);
 *     - Returns 1 (font soft id stub continuum ready). Pure-data
 *       product tag that exclusive wave 13521-13530 surfaces are
 *       present for the font soft path; not a hard font stack runtime
 *       probe.
 *   uint32_t __gj_font_soft_ready_u_13529  (alias)
 *   __libcgj_batch13529_marker = "libcgj-batch13529"
 *
 * Exclusive continuum CREATE-ONLY (13521-13530: font soft id stubs —
 * fontconfig_ok_u_13521, freetype_ok_u_13522, harfbuzz_ok_u_13523,
 * pango_ok_u_13524, cairo_ft_ok_u_13525, fc_pattern_ok_u_13526,
 * fc_cache_ok_u_13527, fc_match_ok_u_13528,
 * font_soft_ready_u_13529, batch_id_13530→13530). Unique surface only;
 * no multi-def. Ok units remain 0; ready→1. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13529_marker[] = "libcgj-batch13529";

/* Soft continuum-ready lamp for font soft id stubs wave. */
#define B13529_FONT_SOFT_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13529_soft_ready(void)
{
	return B13529_FONT_SOFT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_font_soft_ready_u_13529 - font soft id continuum ready.
 *
 * Always returns 1. Soft pure-data product tag that the exclusive
 * wave surfaces are present. Does not call libc. No parent wires.
 */
uint32_t
gj_font_soft_ready_u_13529(void)
{
	(void)NULL;
	return b13529_soft_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_font_soft_ready_u_13529(void)
    __attribute__((alias("gj_font_soft_ready_u_13529")));
