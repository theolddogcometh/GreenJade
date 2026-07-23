/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13530: font soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13530(void);
 *     - Returns the compile-time graph batch number for this TU (13530).
 *   uint32_t __gj_batch_id_13530  (alias)
 *   __libcgj_batch13530_marker = "libcgj-batch13530"
 *
 * Exclusive continuum CREATE-ONLY (13521-13530: font soft id stubs —
 * fontconfig_ok_u_13521, freetype_ok_u_13522, harfbuzz_ok_u_13523,
 * pango_ok_u_13524, cairo_ft_ok_u_13525, fc_pattern_ok_u_13526,
 * fc_cache_ok_u_13527, fc_match_ok_u_13528,
 * font_soft_ready_u_13529, batch_id_13530→13530). Unique surfaces only;
 * no multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13530_marker[] = "libcgj-batch13530";

#define B13530_BATCH_ID  13530u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13530_id(void)
{
	return B13530_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13530 - report this TU's graph batch number.
 *
 * Always returns 13530. Closes the 13521-13530 font soft wave.
 */
uint32_t
gj_batch_id_13530(void)
{
	(void)NULL;
	return b13530_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13530(void)
    __attribute__((alias("gj_batch_id_13530")));
